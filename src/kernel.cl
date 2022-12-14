__kernel void haversine(__global float* lat1,
			__global float* lon1,
			__global float* lat2,
			__global float* lon2,
			__global float* out,
			int WIDTH,int HEIGHT,int s_csv){
	int i = get_global_id(0);
	int j = get_global_id(1);
	int loc = i*HEIGHT+j;

	if((s_csv>0) && (j<=i)) return;

	if(i+1 > WIDTH) return;
	if(j+1 > HEIGHT) return;

	
	float a1r = M_PI_F*lat1[i]/180;
	float a2r = M_PI_F*lat2[j]/180;
	float o1r = M_PI_F*lon1[i]/180; 
	float o2r = M_PI_F*lon2[j]/180;

	float dlon = fabs(o1r-o2r);
	float dlat = fabs(a1r-a2r);
	float a = pow(sin(dlat/2),2.0f) ;
	a += cos(a1r) * cos(a2r) * pow(sin(dlon/2),2.0f);
	float c = 2*asin(sqrt(a));
	out[loc] = 6371*c;
}
