
#include "solution.h"

using namespace std;

ostream& operator << (ostream& out, const TAMSolution& sol)
{
	const vector<TAMSolution::CoreRect>& rects = sol.getCoreRects();

	for(size_t i=0; i<rects.size(); ++i)
		out << "(id:" << rects[i].coreIndex() << ','
			<< rects[i].h() << ',' << rects[i].w() << ")" <<endl;

	const SequencePair& seq_pair = sol.getSequencePair();
	const vector<size_t>& pos_loci = seq_pair.getPositiveLoci();
	out << "<[";
	if( !pos_loci.empty() )
		out << rects[ pos_loci[0] ].coreIndex();
	for(size_t i=1; i<pos_loci.size(); ++i)
		out << "," << rects[ pos_loci[i] ].coreIndex();
	out << "],[";
	const vector<size_t>& neg_loci = seq_pair.getNegativeLoci();
	if( !neg_loci.empty() )
		out << rects[ neg_loci[0] ].coreIndex();
	for(size_t i=1; i<neg_loci.size(); ++i)
		out << "," << rects[ neg_loci[i] ].coreIndex();
	out << "]>";

	return out;
}

bool
TAMSolution::checkTAMWidth(const TWidth& bound) const
{	return checkTAMWidth(bound, _seq_pair);}

bool
TAMSolution::checkTAMWidth(const TWidth& bound, const SequencePair& seq_pair) const
{	// TODO consider to use faster algorithm
	// TODO Reduce repeat code
	vector<TWidth> max_y;
	max_y.resize(seq_pair.size(), 0);
	TWidth new_max_y;
	const vector<size_t>& pos_loci = seq_pair.getPositiveLoci();
	for(vector<size_t>::const_reverse_iterator it = pos_loci.rbegin();
		it != pos_loci.rend(); ++it)
	{	// In reverse order
		const size_t& r_i = *it;
		const size_t& i_neg = seq_pair.findNegative(r_i);

		new_max_y = max_y[ i_neg ] + _rects[r_i].w();

		if(new_max_y > bound) return false;

		for(size_t j=i_neg; j<seq_pair.size(); ++j)
			if(new_max_y > max_y[j]) max_y[j] = new_max_y;
			else break;
	}
	return true;
}

TTime
TAMSolution::getRequiredTime() const
{	// TODO consider to use faster algorithm
	// TODO Reduce repeat code
	TTime new_max_x;
	vector<TTime> max_x;
	max_x.resize(_seq_pair.size(), 0);
	const vector<size_t>& pos_loci = _seq_pair.getPositiveLoci();
	for(vector<size_t>::const_iterator it = pos_loci.begin();
		it != pos_loci.end(); ++it)
	{	// In normal order
		const size_t& r_i = *it;
		const size_t& i_neg = _seq_pair.findNegative(r_i);

		new_max_x = max_x[ i_neg ] + _rects[r_i].h();

		for(size_t j=i_neg; j<_seq_pair.size(); ++j)
			if(new_max_x > max_x[j]) max_x[j] = new_max_x;
			else break;
	}

	return max_x.back();
}

bool
MidSolution::checkPower(const TPower& bound) const
{	return checkPower(bound, _seq_pair);}

bool
MidSolution::checkPower(const TPower& bound, const SequencePair& seq_pair) const
{	// TODO consider to use faster algorithm
	// TODO Reduce repeat code
	vector<TPower> max_y;
	max_y.resize(seq_pair.size(), 0);
	TPower new_max_y;
	const vector<size_t>& pos_loci = seq_pair.getPositiveLoci();
	for(vector<size_t>::const_reverse_iterator it = pos_loci.rbegin();
		it != pos_loci.rend(); ++it)
	{	// In reverse order
		const size_t& r_i = *it;
		const size_t& i_neg = seq_pair.findNegative(r_i);

		new_max_y = max_y[ i_neg ] + _rects[r_i].w();

		if(new_max_y > bound) return false;

		for(size_t j=i_neg; j<seq_pair.size(); ++j)
			if(new_max_y > max_y[j]) max_y[j] = new_max_y;
			else break;
	}
	return true;
}

TTime
MidSolution::getRequiredTime() const
{	// TODO consider to use faster algorithm
	// TODO Reduce repeat code
	vector<TTime> max_x;
	max_x.resize(_seq_pair.size(), 0);
	const vector<size_t>& pos_loci = _seq_pair.getPositiveLoci();
	for(vector<size_t>::const_iterator it = pos_loci.begin();
		it != pos_loci.end(); ++it)
	{	// In normal order
		const size_t& r_i = *it;
		const size_t& i_neg = _seq_pair.findNegative(r_i);

		TTime new_max_x = max_x[ i_neg ] + _rects[r_i].h();

		for(size_t j=i_neg; j<_seq_pair.size(); ++j)
			if(new_max_x > max_x[j]) max_x[j] = new_max_x;
			else break;
	}

	return max_x.back();
}

