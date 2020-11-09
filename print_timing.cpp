#include <clwrap.hpp>

static const char* etstr(enum clwrap::profile_event::evtype et) {
	switch(et) {
	case clwrap::profile_event::EV_MARKER:  return "MARKER";
	case clwrap::profile_event::EV_NDRANGE: return "NDRANGE";
	case clwrap::profile_event::EV_WRITE:   return "WRITE";
	case clwrap::profile_event::EV_READ:    return "READ";
	default: return "UNKNOWN";
	}
}

void print_timing(clwrap  &cw) {
	double offset_sec = -1.0;
	double start_relsec;
	double end_relsec;
	double prev_end_relsec = 0.0;
	double gap_sec;

	cw.finish();

	std::vector<clwrap::profile_event> p_evs = cw.get_p_evs();

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

		if (it->et == clwrap::profile_event::EV_WRITE || it->et == clwrap::profile_event::EV_READ) {
			std::cout << " # argno=" << it->argidx;

			std::cout << "  BW=" <<
				(it->sz / (end_relsec - start_relsec)) * 1e-9 << " GB/s";
		}
		std::cout << std::endl;
	}
	std::cout << "elapsed [sec]: " << end_relsec <<
		" # device timer" <<  std::endl;
}
