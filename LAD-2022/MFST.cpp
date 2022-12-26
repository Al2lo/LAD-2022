﻿#include "stdafx.h"
#include "MFST.h"
#include <iostream>
#include <iomanip>

namespace MFST
{
	int FST_TRACE_n = -1;
	char rbuf[205], sbuf[205], lbuf[1024];
	#define NS(n) GRB::Rule::Chain::N(n);
	#define TS(n)GRB::Rule::Chain::T(n)
	#define ISNS(n) GRB::Rule::Chain::isN(n);

	MfstState::MfstState()
	{
		lenta_position = 0;//позиция на ленте
		nrule = -1;//номер текущего правила
		nrulechain = -1; //номер текущей цепочки
	};
	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrulechain)
	{
		lenta_position = pposition;
		st = pst;					//стек автомата
		nrulechain = -pnrulechain;	
	};
	MfstState::MfstState(short pposition, MFSTSTSTACK pst, short pnrule, short pnrulechain)
	{
		lenta_position = pposition;
		st = pst;
		nrule = pnrule;
		nrulechain = pnrulechain;
	};
	Mfst::MfstDiagnosis::MfstDiagnosis()
	{
		lenta_position = -1;
		rc_step = SURPRISE;
		nrule = -1;
		nrule_chain = -1;
	};
	Mfst::MfstDiagnosis::MfstDiagnosis(short plenta_position, RC_STEP prc_step, short pnrule, short pnrule_chain)
	{
		lenta_position = plenta_position;	
		rc_step = prc_step;			//код завершения шага		
		nrule = pnrule;						
		nrule_chain = pnrule_chain; // номер цепочки правила
	};
	Mfst::Mfst() {
		lenta = 0;
		lenta_size = lenta_position = 0;
	};
	Mfst::Mfst(LexAnalize::LEX plex, GRB::Greibach pgrebach)//(результат работы лексического анализатора, грамматика Грейбах)
	{
		grebach = pgrebach; //Грейбах
		lex = plex; //рез работы лекс анализатора
		lenta = new short[lenta_size = lex.lextab.size];//размер ленты = текущий размер таблицы лексем
		for (int k = 0; k < lenta_size; k++)
			lenta[k] = TS(lex.lextab.table[k].lexema);//заносит в ленту терминалы
		lenta_position = 0;
		st.push(grebach.stbottomT);//добавляет дно стека
		st.push(grebach.startN);//добавляет стартовый символ
		nrulechain = -1;//изначально правило равно -1
	};
	bool Mfst::reststate(const Log::LOG& log) //восстановить состояние автомата
	{
		bool rc = false;
		MfstState state;
		if (rc = (storestate.size() > 0))
		{
			state = storestate.top();
			lenta_position = state.lenta_position;
			st = state.st;
			nrule = state.nrule;
			nrulechain = state.nrulechain;
			storestate.pop();
			MFST_TRACE5(log, "RESSTATE")
				MFST_TRACE2(log)
		};
		return rc;
	};

	Mfst::RC_STEP Mfst::step(const Log::LOG& log) //выполнить шаг автомата
	{
		RC_STEP  rc = SURPRISE;
		if (lenta_position < lenta_size)
		{
			if (GRB::Rule::Chain::isN(st.top()))
			{
				GRB::Rule rule;
				if ((nrule = grebach.getRule(st.top(), rule)) >= 0)
				{
					GRB::Rule::Chain chain;
					if ((nrulechain = rule.getNextChain(lenta[lenta_position], chain, nrulechain + 1)) >= 0)//получаем след цепочку и выводим её номер, или -1
					{
						MFST_TRACE1(log)//вывод
						savestate(log);//сохранить состояние автомата
						st.pop();//извлекаем верхушку стека.
						push_chain(chain); //поместить цепочку правила в стек
						rc = NS_OK; //найдено правило и цепочка... цепочка записана в стек
						MFST_TRACE2(log);//вывод
					}
					else
					{
						MFST_TRACE4(log, "TNS_NORULECHAIN/NS_NORULE")//вывод
						savediagnois(NS_NORULECHAIN); //код завершения
						rc = reststate(log) ? NS_NORULECHAIN : NS_NORULE; //восстановить состояние автомата
					};
				}
				else rc = NS_ERROR;
			}
			else if (st.top() == lenta[lenta_position])
			{
				lenta_position++; st.pop(); nrulechain = -1; rc = TS_OK;
				MFST_TRACE3(cout)
			}
			else
			{
				MFST_TRACE4(log, "TS_NOK/NS_NORULECHAIN")
					rc = reststate(log) ? NS_NORULECHAIN : NS_NORULE;
			};
		}
		else
		{
			rc = LENTA_END;
			MFST_TRACE4(log, "LENTA_END");
		};
		return rc;
	};
	bool Mfst::push_chain(GRB::Rule::Chain chain)
	{
		for (int k = chain.size - 1; k >= 0; k--) st.push(chain.nt[k]);
		return true;
	};
	bool Mfst::savestate(const Log::LOG& log) //сохранить состояние автомата(для отката)
	{
		storestate.push(MfstState(lenta_position, st, nrule, nrulechain)); //стек для сохранения состояния. заносим сохраняемое состояние
		MFST_TRACE6(log, "SAVESTATE:", storestate.size());
		return true;
	};
	bool Mfst::savediagnois(RC_STEP prc_step)
	{
		bool rc = false;
		short k = 0;
		while (k < MFST_DIAGN_digit && lenta_position <= diagnosis[k].lenta_position) k++;
		if (rc = (k < MFST_DIAGN_digit))
		{
			diagnosis[k] = MfstDiagnosis(lenta_position, prc_step, nrule, nrulechain);
			for (short j = k + 1; j < MFST_DIAGN_digit; j++) diagnosis[j].lenta_position = -1;
		};
		return rc;
	};
	bool Mfst::start(const Log::LOG& log)
	{
		bool rc = false;
		RC_STEP rc_step = SURPRISE;
		char buf[MFST_DIAGN_MAXSIZE];
		rc_step = step(log);
		while (rc_step == NS_OK || rc_step == NS_NORULECHAIN || rc_step == TS_OK || rc_step == TS_NOK)
			rc_step = step(log);
		switch (rc_step)
		{
		case LENTA_END:MFST_TRACE4(log, "------>LENTA_END")
			* log.stream << "-------------------------------------------------------------------------------------" << std::endl;
			sprintf_s(buf, MFST_DIAGN_MAXSIZE, "%d:всего строк %d, синтаксический анализ выполнен без ошибок ", 0, lenta_size);
			*log.stream << std::setw(4) << std::left << 0 << ":всего строк " << lenta_size << ", синтаксический анализ выполнен без ошибок " << std::endl;
			rc = true;
			break;
		case NS_NORULE:MFST_TRACE4(log, "------>NS_NORULE")
			* log.stream << "-------------------------------------------------------------------------------------" << std::endl;
			*log.stream << getDiagnosis(0, buf, log) << std::endl;
			*log.stream << getDiagnosis(1, buf, log) << std::endl;
			*log.stream << getDiagnosis(2, buf, log) << std::endl;
			break;
		case NS_NORULECHAIN:MFST_TRACE4(log, "------>NS_NORULECHAIN") break;
		case NS_ERROR:MFST_TRACE4(log, "------>NS_ERROR") break;
		case SURPRISE:MFST_TRACE4(log, "------>SURPRISE") break;
		};
		return rc;

	};
	char* Mfst::getCLenta(char* buf, short pos, short n)
	{
		short i, k = (pos + n < lenta_size) ? pos + n : lenta_size;
		for (i = pos; i < k; i++) buf[i - pos] = GRB::Rule::Chain::alphabet_to_char(lenta[i]);
		buf[i - pos] = 0x00;
		return buf;
	};

	char* Mfst::getDiagnosis(short n, char* buf, const Log::LOG& log)//вывод ошибок
	{
		char v = ' ';
		char* rc = &v;
		int errid = 0;
		int lpos = -1;
		if (n < MFST_DIAGN_digit && (lpos = diagnosis[n].lenta_position) >= 0)
		{
			errid = grebach.getRule(diagnosis[n].nrule).iderror;
			Error::ERROR err = Error::geterror(errid);
			Log::WriteError(log.stream, Error::geterrorin(errid, lex.lextab.table[lpos].sn, 0));
			rc = buf;
		};
		return rc;
	};
	void Mfst::printrules(const Log::LOG& log)//вывод правил
	{
		MfstState state;
		GRB::Rule rule;
		for (unsigned short k = 0; k < storestate.size(); k++)
		{
			state = storestate._Get_container()[k];
			rule = grebach.getRule(state.nrule);
			MFST_TRACE7(log)
		};
	};
	bool Mfst::savededucation()
	{
		MfstState state;
		GRB::Rule rule;
		deducation.nrules = new short[deducation.size = storestate.size()];
		deducation.nrulechains = new short[deducation.size];
		for (unsigned short k = 0; k < storestate.size(); k++)
		{
			state = storestate._Get_container()[k];
			deducation.nrules[k] = state.nrule;
			deducation.nrulechains[k] = state.nrulechain;
		};
		return true;
	};
	char* Mfst::getCSt(char* buf)
	{
		for (int k = (signed)st.size() - 1; k >= 0; --k)
		{
			short p = st._Get_container()[k];
			buf[st.size() - 1 - k] = GRB::Rule::Chain::alphabet_to_char(p);
		};
		buf[st.size()] = 0x00;
		return buf;
	};
};