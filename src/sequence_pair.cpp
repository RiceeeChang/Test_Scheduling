#include <algorithm>
#include <cassert>

#include "sequence_pair.h"

using namespace std;

ostream& operator << (ostream& out, const SequencePair& seq_pair)
{	
	const vector<size_t>& pos_loci = seq_pair.getPositiveLoci();
	out << "<[";
	if( !pos_loci.empty() )
		out << pos_loci[0];
	for(size_t i=1; i<pos_loci.size(); ++i)
		out << "," << pos_loci[i];
	out << "],[";
	const vector<size_t>& neg_loci = seq_pair.getNegativeLoci();
	if( !neg_loci.empty() )
		out << neg_loci[0];
	for(size_t i=1; i<neg_loci.size(); ++i)
		out << "," << neg_loci[i];
	out << "]>";

	return out;
}

void
SequencePair::swap(SequencePair& a, SequencePair& b)
{
	assert(a.size() == b.size());
	std::swap(a._loci_pos, b._loci_pos);
	std::swap(a._loci_neg, b._loci_neg);
	std::swap(a._loci_idx, b._loci_idx);
}

SequencePair::SequencePair(
	const std::vector<size_t>& pos,
	const std::vector<size_t>& neg
): _loci_pos(pos), _loci_neg(neg)
{
	assert(pos.size() == neg.size());
	_loci_idx.resize( pos.size() );
	for(size_t i=0; i<pos.size(); ++i)
	{
		const size_t& a = pos[i];
		const size_t& b = neg[i];
		_loci_idx[a]._pos = i;
		_loci_idx[b]._neg = i;
	}
#ifdef DEBUG
	for(size_t i=0; i<_loci_idx.size(); ++i)
	{
		assert(_loci_pos[ _loci_idx[i]._pos ] == i);
		assert(_loci_neg[ _loci_idx[i]._neg ] == i);
	}
#endif
}

void SequencePair::swapPositive(const size_t& a, const size_t& b)
{
	size_t& a_pos = _loci_idx[a]._pos;
	size_t& b_pos = _loci_idx[b]._pos;
	std::swap(_loci_pos[a_pos], _loci_pos[b_pos]);
	std::swap(a_pos, b_pos);

	assert(_loci_pos[a_pos] == a);
	assert(_loci_pos[b_pos] == b);
}

void SequencePair::swapNegative(const size_t& a, const size_t& b)
{
	size_t& a_neg = _loci_idx[a]._neg;
	size_t& b_neg = _loci_idx[b]._neg;
	std::swap(_loci_neg[a_neg], _loci_neg[b_neg]);
	std::swap(a_neg, b_neg);

	assert(_loci_neg[a_neg] == a);
	assert(_loci_neg[b_neg] == b);
}

void SequencePair::swapBoth(const size_t& a, const size_t& b)
{
	swapPositive(a, b);
	swapNegative(a, b);
}
