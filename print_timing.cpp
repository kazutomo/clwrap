#include <clwrap.hpp>
#include <numeric>
#include <algorithm>
#include <math.h>

using namespace std;


static double calc_mean(const vector<double> &v)
{
	return accumulate(v.begin(), v.end(), 0.0) / (double)v.size();
}

static double calc_std(const vector<double> &v)
{
	vector<double> diff(v.size());
	double mean = calc_mean(v);

	transform(v.begin(), v.end(), diff.begin(),
		  [mean](double x) { return x - mean; });

        double sq_sum = inner_product(diff.begin(), diff.end(),
                                    diff.begin(), 0.0);

        return sqrt(sq_sum/v.size());
}


static const char* etstr(enum clwrap::profile_event::evtype et) {
	switch(et) {
	case clwrap::profile_event::EV_MARKER:  return "MARKER";
	case clwrap::profile_event::EV_NDRANGE: return "NDRANGE";
	case clwrap::profile_event::EV_WRITE:   return "WRITE";
	case clwrap::profile_event::EV_READ:    return "READ";
	default: return "UNKNOWN";
	}
}

// this function cannot be generalized. it is specific to copybenchhost
// the function name should be updated accordingly.
void print_timing(clwrap  &cw) {
	double offset_sec = -1.0;
	double start_relsec;
	double end_relsec;
	double prev_end_relsec = 0.0;
	double gap_sec;
	size_t total_wr_sz = 0;

	vector<double>  l_rd, l_ex, l_wr; // latency
	vector<double>  b_rd, b_ex, b_wr; // bandwidth

	cw.finish();

	std::vector<clwrap::profile_event> p_evs = cw.get_p_evs();

	auto lprintBW = [](double bw)
		       {
			       cout << " BW="
				    << (bw*1e-9)
				    << " GB/s";
		       };

	for (std::vector<clwrap::profile_event>::const_iterator it = p_evs.begin(); it != p_evs.end(); ++it) {
		std::printf("%-8s", etstr(it->et));
		if (offset_sec < 0.0) offset_sec = it->start_sec;
		start_relsec = it->start_sec - offset_sec;
		end_relsec = it->end_sec - offset_sec;
		if(prev_end_relsec == 0.0) gap_sec = 0.0;
		else gap_sec = start_relsec - prev_end_relsec;

		std::printf("d=%9.7f s=%9.7f e=%9.7f g=%9.7f [sec]",
			    (end_relsec - start_relsec),
			    start_relsec, end_relsec,
			    gap_sec
			    );

		prev_end_relsec = end_relsec;

		double drelsec = end_relsec - start_relsec;
		if (it->et == clwrap::profile_event::EV_WRITE) {
			double bw = it->sz/drelsec;
			lprintBW(bw);
			l_wr.push_back(gap_sec);
			b_wr.push_back(bw);
			total_wr_sz += it->sz;
		} else if (it->et == clwrap::profile_event::EV_READ) {
			double bw = it->sz/drelsec;
			lprintBW(bw);
			l_rd.push_back(gap_sec);
			b_rd.push_back(bw);
		} else if (it->et == clwrap::profile_event::EV_NDRANGE) {
			// total_wr_sz is the total of write to device
			// '*2' because the kernel copies memory
			double bw = total_wr_sz*2/drelsec;
			lprintBW(bw);
			l_ex.push_back(gap_sec);
			b_ex.push_back(bw);
		}
		cout << endl;
	}

#if 0
	std::printf("HOST2DEV :  BW %8.3f(%8.3f) GB/s / latency %7.2f(%7.2f) [usec]\n", calc_mean(b_wr)*1e-9, calc_std(b_wr)*1e-9,
		    calc_mean(l_wr)*1e+6, calc_std(l_wr)*1e+6);
	std::printf("DEV2HOST :  BW %8.3f(%8.3f) GB/s / latency %7.2f(%7.2f) [usec]\n", calc_mean(b_rd)*1e-9, calc_std(b_rd)*1e-9,
		    calc_mean(l_rd)*1e+6, calc_std(l_rd)*1e+6);
	std::printf("DEVICEMEM:  BW %8.3f(%8.3f) GB/s / latency %7.2f(%7.2f) [usec]\n", calc_mean(b_ex)*1e-9, calc_std(b_ex)*1e-9,
		    calc_mean(l_ex)*1e+6, calc_std(l_ex)*1e+6);
	cout << "NOTE: mean(std). The number in the parenthesis is standard deviation" << endl;
#endif
	std::printf("HOST2DEV %8.3f %8.3f  %7.2f %7.2f\n", calc_mean(b_wr)*1e-9, calc_std(b_wr)*1e-9,
		    calc_mean(l_wr)*1e+6, calc_std(l_wr)*1e+6);
	std::printf("DEV2HOST %8.3f %8.3f  %7.2f %7.2f\n", calc_mean(b_rd)*1e-9, calc_std(b_rd)*1e-9,
		    calc_mean(l_rd)*1e+6, calc_std(l_rd)*1e+6);
	std::printf("DEVICEMEM  %8.3f  %8.3f  %7.2f  %7.2f\n", calc_mean(b_ex)*1e-9, calc_std(b_ex)*1e-9,
		    calc_mean(l_ex)*1e+6, calc_std(l_ex)*1e+6);



	cout << "elapsed [sec]: " << end_relsec <<
		" # device timer" << endl;
}
