#include "Implicant.h"

#include <vector>
#include <string>

using namespace std;

Implicant::Implicant(vector<int> values):
	numOfVariables(values.size()),
	numOfTrues(0),
	numOfDontCares(0),
	values(values),
	valid(true),
	marked(false)
	//minterms()
{
	for(unsigned int i=0; i<values.size(); i++)
	{
		switch(values[i])
		{
		case TRUE:
			numOfTrues++;
			break;
		case DONT_CARE:
			numOfDontCares++;
			break;
		case INVALID:
			this->valid = false;
		}
	}
}

bool Implicant::merge(const Implicant& first, const Implicant& second)
{
	if(first.numOfVariables!=second.numOfVariables
		||first.numOfDontCares!=second.numOfDontCares
		||first.numOfTrues!=second.numOfTrues-1)
	{
		return false;
	}

	(*this) = first;
	numOfDontCares++;
	marked = false;
	mintermIDs.insert(this->mintermIDs.end(), second.mintermIDs.begin(), second.mintermIDs.end());

	bool differenceFound(false);
	for(unsigned int i=0; i<values.size(); i++)
	{
		if(first.values[i] != second.values[i])
		{
			if(!differenceFound && first.values[i]==FALSE && second.values[i]==TRUE)
			{
				differenceFound = true;
				this->values[i] = DONT_CARE;
			}
			else return false;
		}
	}
	return true;
}

string Implicant::toString()
{
	string result;
	result.reserve(2*numOfVariables);
	for(unsigned int i=0; i<numOfVariables; i++)
	{
		if(values[i]==TRUE)
		{
			result += '+'; 
		}
		else if(values[i]==FALSE)
		{
			result += '-'; 
		}
		else if(values[i]==DONT_CARE)
		{
			result += 'x'; 
		}
	}
	return result;
}


list<Implicant> Implicant::toMintermList()
{
	list<Implicant> implicantList;
	addMintermsToListFromPosition(0, implicantList);
	return implicantList;
}

void Implicant::addMintermsToListFromPosition(unsigned int position, std::list<Implicant>& mintermList)
{
	for(unsigned int i=position; i<values.size(); i++)
	{
		if(values[i]==Implicant::DONT_CARE)
		{
			values[i] = Implicant::TRUE;
			Implicant firstImplicant(values);
			firstImplicant.addMintermsToListFromPosition(i+1, mintermList);

			values[i] = Implicant::FALSE;
			Implicant secondImplicant(values);
			secondImplicant.addMintermsToListFromPosition(i+1, mintermList);

			//visszaállítjuk az eredeti értéket:
			values[i] = Implicant::DONT_CARE;
			return;
		}
		else if(values[i]==Implicant::INVALID)
		{
			return;
		}
	}

	//ha végigértünk a változókon, mintermet kaptunk
	mintermList.push_back(*this);
}

bool Implicant::operator==(const Implicant& other) const
{
	if(this->numOfVariables!=other.numOfVariables
		|| this->numOfTrues!=other.numOfTrues
		|| this->numOfDontCares!=other.numOfDontCares)
	{
		return false;
	}

	for(unsigned int i=0; i<values.size();i++)
	{
		if(this->values[i]!=other.values[i])
		{
			return false;
		}
	}
	return true;
}


bool Implicant::operator<(const Implicant& other) const
{
	return lexicographical_compare(this->values.begin(), this->values.end(), other.values.begin(), other.values.end());
}