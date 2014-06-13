#ifndef SEQUENCE_PAIR_H
#define SEQUENCE_PAIR_H

#include <iostream>
#include <vector>

class SequencePair
{
public:
	static void swap(SequencePair&, SequencePair&);

private:
	struct LociIdx
	{	size_t _pos, _neg;};
public:
	SequencePair(
		const std::vector<size_t>&,
		const std::vector<size_t>&
	);
	~SequencePair(){}

	void swapPositive(const size_t&, const size_t&);
	void swapNegative(const size_t&, const size_t&);
	void swapBoth(const size_t&, const size_t&);

	size_t size() const {return _loci_idx.size();}
	const std::vector<size_t>& getPositiveLoci() const
	{	return _loci_pos;}
	const std::vector<size_t>& getNegativeLoci() const
	{	return _loci_neg;}

	const size_t& findPositive(const size_t& a) const
	{	return _loci_idx[a]._pos;}
	const size_t& findNegative(const size_t& a) const
	{	return _loci_idx[a]._neg;}

	// check if a at b's left
	bool left(const size_t& a, const size_t& b) const
	{	return _loci_idx[a]._pos < _loci_idx[b]._pos
			&& _loci_idx[a]._neg < _loci_idx[b]._neg;
	}
	bool below(const size_t& a, const size_t& b) const
	{	return _loci_idx[a]._pos > _loci_idx[b]._pos
			&& _loci_idx[a]._neg < _loci_idx[b]._neg;
	}

private:
	std::vector<size_t> _loci_pos, _loci_neg;
	std::vector<LociIdx> _loci_idx;
};

std::ostream& operator << (std::ostream&, const SequencePair&);

#endif
