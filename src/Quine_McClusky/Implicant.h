#ifndef IMPLICANT_H
#define IMPLICANT_H

#include <vector>
#include <list>
#include <string>

class Implicant
{
public://private:
	unsigned int numOfVariables;
	unsigned int numOfTrues;
	unsigned int numOfDontCares;
	std::vector<int> values;
	bool valid;

	void addMintermsToListFromPosition(unsigned int position, std::list<Implicant>& mintermList);
	std::list<unsigned int> mintermIDs;
	mutable bool marked;
public:
	enum Value{INVALID=0, TRUE=1, FALSE=2, DONT_CARE=3};

	Implicant(std::vector<int> values = std::vector<int>());
	//void addMintermID(unsigned int);

	bool merge(const Implicant& first, const Implicant& second);
	std::list<Implicant> toMintermList();
	std::string toString();

	Implicant& operator=(const Implicant& other);
	bool operator==(const Implicant& other) const;
	bool operator<(const Implicant& other) const;
	Implicant operator&(const Implicant& other) const;
};

#endif