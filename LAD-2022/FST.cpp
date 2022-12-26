#include "stdafx.h"

namespace FST
{

	RELATION::RELATION(char sym, short num)
	{
		symbol = sym;
		nnode = num;
	};

	NODE::NODE()
	{
		n_relation = 0;
		RELATION* relations = NULL;
	};

	NODE::NODE(short n, RELATION rel, ...)
	{
		n_relation = n;
		RELATION* p = &rel;
		relations = new RELATION[n];
		for (int i = 0; i < n; i++)
		{
			relations[i] = p[i];
		}
	}

	FST::FST(short ns, NODE n, ...)
	{
		this->nodes = new NODE[ns];
		NODE* temp = &n;
		this->nstates = ns;
		this->rstates = new short[ns];
		for (short i = 0; i < ns; i++)
			this->nodes[i] = *(temp + i);
	}

	FST::FST(char* s, FST& fst)
	{
		this->nodes = new NODE[fst.nstates];
		NODE* temp = fst.nodes;
		this->string = s;
		this->nstates = fst.nstates;
		this->rstates = new short[this->nstates];
		for (short i = 0; i < this->nstates; i++)
			this->nodes[i] = *(temp + i);
	}

	bool execute(FST& fst)
	{
		int i, j;
		memset(fst.rstates, -1, fst.nstates * sizeof(short)); // Заполняем fst.rstates массив -1-ами размером  fst.nstates * sizeof(short)
		for (fst.rstates[0] = 0, fst.position = 0; fst.position < (signed)(strlen(fst.string)); fst.position++)
		{
			for (i = 0; i < fst.nstates; i++)
			{
				if (fst.rstates[i] == fst.position)
				{
					for (j = 0; j < fst.nodes[i].n_relation; j++) //релейшены 
					{
						if (fst.nodes[i].relations[j].symbol == fst.string[fst.position])
						{
							fst.rstates[fst.nodes[i].relations[j].nnode] = fst.position + 1;
						}
					};
				};
			};
		};
		return (fst.rstates[fst.nstates - 1] == (strlen(fst.string))); // Совпадает ли конечная позиция с длиной строки
	};
}

