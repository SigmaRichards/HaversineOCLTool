#include <unistd.h>

struct input_args{
	bool verbose = false;
	int ret_status = 0;
	bool col_heads = true;
	bool row_heads = true;

	bool single_csv = false;
	bool nearest_only = false;
	
	std::string csv0 = "";
	std::string csv1 = "";

	int lat0_ind = 0;
	std::string lat0_name = "";
	int lon0_ind = 1;
	std::string lon0_name = "";
	
	int lat1_ind = 0;
	std::string lat1_name = "";
	int lon1_ind = 1;
	std::string lon1_name = "";
};

void print_args(struct input_args inp){
	std::cout<<"Column Headers: "<<inp.col_heads<<"\n";
	std::cout<<"Row Headers: "<<inp.row_heads<<"\n";
	std::cout<<"Single CSV Mode: "<<inp.single_csv<<"\n";
	std::cout<<"Nearest Only Mode: "<<inp.nearest_only<<"\n";
	std::cout<<"CSV1 - \n";
	std::cout<<"       Name: "<<inp.csv0<<"\n";
	std::cout<<"       Lat head: "<<inp.lat0_name<<"\n";
	std::cout<<"       Lat ind: "<<inp.lat0_ind<<"\n";
	std::cout<<"       Lon head: "<<inp.lon0_name<<"\n";
	std::cout<<"       Lon ind: "<<inp.lon0_ind<<"\n";
	std::cout<<"CSV2 - \n";
	std::cout<<"       Name: "<<inp.csv1<<"\n";
	std::cout<<"       Lat head: "<<inp.lat1_name<<"\n";
	std::cout<<"       Lat ind: "<<inp.lat1_ind<<"\n";
	std::cout<<"       Lon head: "<<inp.lon1_name<<"\n";
	std::cout<<"       Lon ind: "<<inp.lon1_ind<<"\n";
	std::cout<<std::endl;
}

bool is_digits(const std::string &str){
    return std::all_of(str.begin(), str.end(), ::isdigit);
}



struct input_args parse_inputs(int argc, char** argv){
	input_args output;
	
	opterr = 0;
	int c;
	while ((c = getopt (argc, argv, "vcrnsa:b:A:B:i:j:I:J:")) != -1){
		switch(c){
			case 'v':
				output.verbose = true;
				break;
			case 'c':
				output.col_heads = false;
				break;
			case 'r':
				output.row_heads = false;
				break;
			case 'n':
				output.nearest_only = true;
				break;
			case 's':
				output.single_csv = true;
				break;
			case 'a':
				output.lat0_name = optarg;
				break;
			case 'b':
				output.lon0_name = optarg;
				break;
			case 'A':
				output.lat1_name = optarg;
				break;
			case 'B':
				output.lon1_name = optarg;
				break;
			case 'i':
				if (is_digits(optarg)) {
					output.lat0_ind = std::stoi(optarg);
				} else {
					fprintf(stderr,"Argument for `%c' must be digit.\n",optopt);
					output.ret_status = 1;
					return output;
				}
				break;
			case 'j':
				if (is_digits(optarg)) {
					output.lon0_ind = std::stoi(optarg);
				} else {
					fprintf(stderr,"Argument for `%c' must be digit.\n",optopt);
					output.ret_status = 1;
					return output;
				}
				break;
			case 'I':
				if (is_digits(optarg)) {
					output.lat1_ind = std::stoi(optarg);
				} else {
					fprintf(stderr,"Argument for `%c' must be digit.\n",optopt);
					output.ret_status = 1;
					return output;
				}
				break;
			case 'J':
				if (is_digits(optarg)) {
					output.lon1_ind = std::stoi(optarg);
				} else {
					fprintf(stderr,"Argument for `%c' must be digit.\n",optopt);
					output.ret_status = 1;
					return output;
				}
				break;
			case '?':
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				output.ret_status = 1;
				return output;
			default:
				break;
		}
	}
	bool is_valid_single = output.single_csv && (argc - optind == 1);
	bool is_valid_double = (!output.single_csv) && (argc - optind == 2);

	if (output.single_csv){
		if (!is_valid_single){
			fprintf(stderr,"Expect exactly 1 non-option arguments using `-s'.");
			output.ret_status = 1;
		} else{
			output.csv0 = argv[optind];
		}
		return output;
	}
	//Early return for single output - only double output
	if(!is_valid_double){
		fprintf(stderr,"Expext exactly 2 non-option arguments. If you only want 1 CSV evlautated use `-s'.");
		output.ret_status = 1;
		return output;
	}
	output.csv0 = argv[optind];
	output.csv1 = argv[optind+1];
	return output;
}

