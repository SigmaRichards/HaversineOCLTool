#include <iostream>
#include <string>
#include <fstream>
#include "rapidcsv.h"//Vector defined in here

#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

int cl_error_report(cl_int retv, std::string add = ""){
	if(retv<0){
		std::cout<<"OpenCL reported an error."<<std::endl;
		std::cout<<"Error code: "<<retv<<std::endl;
	}else{
		std::cout<<"General error code: "<<retv<<std::endl;
	}
	if(add!=""){
		std::cout<<"Additional note: "<<add<<std::endl;
	}
	return (int)retv;
}


std::vector<size_t> determine_gs(int l1,int l2){
	std::vector<size_t> out;
	int v1 = 32;
	if((l1*l2)%v1==0){
		out.push_back((size_t)l1);
		out.push_back((size_t)l2);
	}else{
		int v2 = 1;
		if(l1>l2){
			while(((l1+v2)*l2)%v1!=0) v2++;
			out.push_back((size_t)(l1+v2));
			out.push_back((size_t)l2);
		}else{
			while((l1*(l2+v2))%v1!=0) v2++;
			out.push_back((size_t)l1);
			out.push_back((size_t)(l2+v2));
		}
	}
	return out;
}
std::vector<size_t> determine_ls(std::vector<size_t> gs){
	int v1 = 32;
	int v2 = 1;
	while((gs[0]%v1!=0)|(gs[1]%v2!=0)){
		v1 = (int)(v1/2);
		v2*=2;
	}
	std::vector<size_t> out = {(size_t)v1,(size_t)v2};
	return out;
}

int run_cl_haver(){

	//Temp data for testing
	std::vector<float> tmp_latdata1 = {1.1,2.2,3.3};
	std::vector<float> tmp_londata1 = {1.0,1.1,1.2};
	std::vector<float> tmp_latdata2 = {1.1,2.2,3.3,4.4};
	std::vector<float> tmp_londata2 = {1.1,2.2,3.3,4.4};

	int l1 = tmp_latdata1.size();
	int l2 = tmp_latdata2.size();

	int WIDTH = l1;
	int HEIGHT = l2;

	std::vector<float> out_data(l1*l2);
	
	float* lat1 = tmp_latdata1.data();
	float* lon1 = tmp_londata1.data();
	float* lat2 = tmp_latdata2.data();
	float* lon2 = tmp_londata2.data();
	float* out = out_data.data();

	cl_int ret;

	//Read Kernel Source - TODO
	std::fstream kern_file("src/kernel.cl");
	std::vector<std::string> kern_src;
	std::string kern_line;
	while(getline(kern_file,kern_line)){
		kern_src.push_back(kern_line+'\n');
	}
	kern_file.close();
	
	std::vector<const char*> kern_src_p;
	int kern_count = kern_src.size();
	std::vector<size_t> kern_lengths;
	for(int i = 0; i < kern_count; i++){
		kern_src_p.push_back(kern_src[i].c_str());
		kern_lengths.push_back(kern_src[i].length());
	}



	
	//Get Device ID
	// - clGetPlatforms
	// - clGetDeviceIDs
	cl_uint num_dev;//Resused for both platform and device
	cl_platform_id plat_id;
	ret = clGetPlatformIDs(1,&plat_id,&num_dev);
	if(ret!=CL_SUCCESS) return cl_error_report(ret);
	if(num_dev<1) return cl_error_report(1,"No OpenCL platforms found.");
	
	cl_device_id dev_id;
	ret = clGetDeviceIDs(plat_id, CL_DEVICE_TYPE_ALL,1,&dev_id,&num_dev);
	if(ret!=CL_SUCCESS) return cl_error_report(ret);
	if(num_dev<1) return cl_error_report(1,"No OpenCL devices found on default platform.");


	//
	//Create Context
	// - clCreateConxt
	
	cl_context context = clCreateContext(NULL,1,&dev_id,NULL,NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret);

	//
	//Create Command Queue
	// - clCreateCommandQueue
	cl_command_queue comQ = clCreateCommandQueueWithProperties(context,dev_id,NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret);
	

	//Create memory buffers
	// - clCreateBuffer
	cl_mem c_a1 = clCreateBuffer(context,CL_MEM_READ_ONLY,l1*sizeof(float),NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Lat1 CL_MEM error.");
	cl_mem c_o1 = clCreateBuffer(context,CL_MEM_READ_ONLY,l1*sizeof(float),NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Lon1 CL_MEM error.");
	cl_mem c_a2 = clCreateBuffer(context,CL_MEM_READ_ONLY,l2*sizeof(float),NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Lat2 CL_MEM error.");
	cl_mem c_o2 = clCreateBuffer(context,CL_MEM_READ_ONLY,l2*sizeof(float),NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Lon2 CL_MEM error.");
	cl_mem c_ot = clCreateBuffer(context,CL_MEM_READ_ONLY,l1*l2*sizeof(float),NULL,&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"OUT CL_MEM error.");
	
	
	//Copy data to buffers
	// - clEnqueueWriteBuffer
	ret = clEnqueueWriteBuffer(comQ,c_a1,CL_TRUE,(size_t)0,l1*sizeof(float),(void*)lat1,0,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"A1 CL-write error");
	ret = clEnqueueWriteBuffer(comQ,c_o1,CL_TRUE,(size_t)0,l1*sizeof(float),(void*)lon1,0,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"A1 CL-write error");
	ret = clEnqueueWriteBuffer(comQ,c_a2,CL_TRUE,(size_t)0,l2*sizeof(float),(void*)lat2,0,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"A1 CL-write error");
	ret = clEnqueueWriteBuffer(comQ,c_o2,CL_TRUE,(size_t)0,l2*sizeof(float),(void*)lon2,0,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"A1 CL-write error");

	//Create program
	// - clCreateProgramWithSource
	cl_program program = clCreateProgramWithSource(context,kern_src_p.size(),kern_src_p.data(),kern_lengths.data(),&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"CreateProgramWithSource error");
	//Build program
	// - clBuildProgram
	ret = clBuildProgram(program,1,&dev_id,NULL,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"BuildProgram error");
	
	//Create kernel
	// - clCreateKernel
	cl_kernel kernel = clCreateKernel(program,"haversine",&ret);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Create kernel error");

	//Set kernel arguments
	// - clSetkKernelArg
	ret = clSetKernelArg(kernel,0,sizeof(cl_mem),(void*)&c_a1);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg c_a1");
	ret = clSetKernelArg(kernel,1,sizeof(cl_mem),(void*)&c_o1);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg c_o1");
	ret = clSetKernelArg(kernel,2,sizeof(cl_mem),(void*)&c_a2);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg c_a2");
	ret = clSetKernelArg(kernel,3,sizeof(cl_mem),(void*)&c_o2);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg c_o2");
	ret = clSetKernelArg(kernel,4,sizeof(cl_mem),(void*)&c_ot);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg c_ot");
	ret = clSetKernelArg(kernel,5,sizeof(int),(void*)&WIDTH);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg WIDTH");
	ret = clSetKernelArg(kernel,6,sizeof(int),(void*)&HEIGHT);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"clSetKernelArg HEIGHT");


	//Execute
	// - clEnqueueNDRangeKernel
	// - clEnqueueReadBuffer
	
	std::vector<size_t> gs = determine_gs(l1,l2);
	std::vector<size_t> ls = determine_ls(gs);

	ret = clEnqueueNDRangeKernel(comQ,kernel,2,0,gs.data(),ls.data(),0,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Executing kernel");

	clEnqueueReadBuffer(comQ,c_ot,CL_TRUE,0,l1*l2*sizeof(float),(void*)out,0,NULL,NULL);
	if(ret!=CL_SUCCESS) return cl_error_report(ret,"Reading data from device.");

	//Clean up
	clFlush(comQ);
	clFinish(comQ);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseMemObject(c_a1);
	clReleaseMemObject(c_o1);
	clReleaseMemObject(c_a2);
	clReleaseMemObject(c_o2);
	clReleaseMemObject(c_ot);
	clReleaseCommandQueue(comQ);
	clReleaseContext(context);

	std::cout<<out[0]<<std::endl;
	return 0;
}



int main(int argc, char** argv){

	//Assuming argument 1 is the csv
	//Assuming the csv has row and column headers
	//Row headers used as labels
	//Assuming lat then lon
	//Assuming the literal names "Lat" and "Lon"
	
	
	rapidcsv::Document doc0(argv[1], rapidcsv::LabelParams(0, 0));
	rapidcsv::Document doc1(argv[1], rapidcsv::LabelParams(0, 0));
	
	std::vector<float> lat0 = doc0.GetColumn<float>("Lat");
	std::vector<float> lon0 = doc0.GetColumn<float>("Lon");
	
	std::vector<float> lat1 = doc1.GetColumn<float>("Lat");
	std::vector<float> lon1 = doc1.GetColumn<float>("Lon");

	int v = run_cl_haver();
	return v;
}
