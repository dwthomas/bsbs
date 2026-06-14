// Copyright © 2013 the Search Authors under the MIT license. See AUTHORS for the list of authors.
#include "../utils/geom2d.hpp"
#include "search.hpp"
#include "astar.hpp"
#include "wastar.hpp"
#include "greedy.hpp"
#include "beam.hpp"
#include "bead.hpp"
#include "hhatgreedy.hpp"
#include "ees.hpp"
#include "rrd.hpp"
#include "rrdnofocal.hpp"
#include "rrdnoopen.hpp"
#include "bsbs.hpp"
#include "bsbsflayer.hpp"
#include "bsbsfill.hpp"
#include "../utils/utils.hpp"
#include <cstddef>
#include <cstdio>

// Functions for conveniently defining a new main
// function for a domain.

void dfheader(FILE*);
void dffooter(FILE*);
void dfpair(FILE *, const char *, const char *, ...);
void dfprocstatus(FILE*);
void fatal(const char*, ...);

template<class D> SearchAlgorithm<D> *getsearch(int argc, const char *argv[]);

template<class D> Result<D> search(D &d, int argc, const char *argv[]) {
	return searchGet(getsearch, d, argc, argv);
}

template<class D> Result<D> searchGet(SearchAlgorithm<D>*(*get)(int, const char *[]), D &d, int argc, const char *argv[]) {
	SearchAlgorithm<D> *srch = get(argc, argv);
	if (!srch && argc > 1)
		fatal("Unknow search algorithm: %s", argv[1]);
	if (!srch)
		fatal("Must specify a search algorithm");

	typename D::State s0 = d.initialstate();
	dfpair(stdout, "initial heuristic", "%f", (double) d.h(s0));
	dfpair(stdout, "initial distance", "%f", (double) d.d(s0));
	dfpair(stdout, "algorithm", argv[1]);

	try {
		srch->search(d, s0);
	} catch (std::bad_alloc&) {
		dfpair(stdout, "out of memory", "%s", "true");
		srch->res.path.clear();
		srch->res.ops.clear();
		srch->finish();
	}
	if (srch->res.path.size() > 0) {
		dfpair(stdout, "final sol cost", "%f",
			(double) d.pathcost(srch->res.path, srch->res.ops));
	} else {
		dfpair(stdout, "final sol cost", "%f", -1.0);
	}
	srch->output(stdout);

	Result<D> res = srch->res;
	delete srch;

	return res;
}

template<class D> SearchAlgorithm<D> *getsearch(int argc, const char *argv[]) {
	if (argc < 2)
		fatal("No algorithm specified");

	
    if (strcmp(argv[1], "astar") == 0)
		return new Astar<D>(argc, argv);
	else if (strcmp(argv[1], "wastar") == 0)
		return new Wastar<D>(argc, argv);
	else if (strcmp(argv[1], "greedy") == 0)
		return new Greedy<D>(argc, argv);
	else if (strcmp(argv[1], "speedy") == 0)
		return new Greedy<D, true>(argc, argv);
	else if (strcmp(argv[1], "beam") == 0)
		return new BeamSearch<D>(argc, argv);
	else if (strcmp(argv[1], "bead") == 0)
		return new BeadSearch<D>(argc, argv);
	else if (strcmp(argv[1], "hhatgreedy") == 0)
		return new Hhatgreedy<D>(argc, argv);
	else if (strcmp(argv[1], "ees") == 0)
		return new EES<D>(argc, argv);
	else if (strcmp(argv[1], "rrd") == 0)
		return new RRD<D>(argc, argv);
	else if (strcmp(argv[1], "rrdnofocal") == 0)
		return new RRDNoFocal<D>(argc, argv);
	else if (strcmp(argv[1], "rrdnoopen") == 0)
		return new RRDNoOpen<D>(argc, argv);
	else if (strcmp(argv[1], "bsbs") == 0)
		return new BSBS<D>(argc, argv);
	else if (strcmp(argv[1], "bsbsflayer") == 0)
		return new BSBSFLAYER<D>(argc, argv);
	else if (strcmp(argv[1], "bsbsfill") == 0)
		return new BSBSFILL<D>(argc, argv);
	
	fatal("Unknown algorithm: %s", argv[1]);
	return NULL;	// Unreachable
}
