// QuantBox_XAPI_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if defined WINDOWS || _WIN32
#include <Windows.h>
#endif
#include <time.h>

#include <string.h>
#include "../include/XApiCpp.h"

#ifdef _WIN64
#pragma comment(lib, "../lib/QuantBox_XAPI.lib")
#else
#pragma comment(lib, "../lib/QuantBox_XAPI.lib")
#endif

class CXSpiImpl :public CXSpi
{
public:
	virtual void OnConnectionStatus(ConnectionStatus status, RspUserLoginField* pUserLogin, int size1)
	{
		printf("%d\r\n", status);
		if (status == ConnectionStatus::Done)
		{
			// 得到当前的时间
			time_t rawtime;
			struct tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			int mon = timeinfo->tm_mon;
			char buf[64] = { 0 };
			for (int i = 0; i < 12; ++i)
			{
				int x = mon + i;
				int a = x / 12;
				int b = x % 12;
				sprintf(buf, "IF%d%02d", (1900 + timeinfo->tm_year + a) % 100, b + 1);
				m_pApi->Subscribe(buf, "");
				sprintf(buf, "TF%d%02d", (1900 + timeinfo->tm_year + a) % 100, b + 1);
				m_pApi->Subscribe(buf, "");
			}
		}
	}
	virtual void OnRtnError(ErrorField* pError){};

	virtual void OnRtnDepthMarketData(DepthMarketDataField* pMarketData)
	{
		printf("%s,%f\r\n", pMarketData->Symbol,pMarketData->LastPrice);
	}

	virtual void OnRtnQuoteRequest(QuoteRequestField* pQuoteRequest){};

	virtual void OnRspQryInstrument(InstrumentField* pInstrument, int size1, bool bIsLast){};
	virtual void OnRspQryTradingAccount(AccountField* pAccount, int size1, bool bIsLast){};
	virtual void OnRspQryInvestorPosition(PositionField* pPosition, int size1, bool bIsLast){};
	virtual void OnRspQrySettlementInfo(SettlementInfoField* pSettlementInfo, int size1, bool bIsLast){};
	virtual void OnRspQryInvestor(InvestorField* pInvestor, int size1, bool bIsLast){};
	virtual void OnRtnOrder(OrderField* pOrder)
	{
		printf("%d,%s\r\n", pOrder->ErrorID,pOrder->Text);
	}
	virtual void OnRtnTrade(TradeField* pTrade){};
	virtual void OnRtnQuote(QuoteField* pQuote){};

	virtual void OnRspQryHistoricalTicks(TickField* pTicks, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};
	virtual void OnRspQryHistoricalBars(BarField* pBars, int size1, HistoricalDataRequestField* pRequest, int size2, bool bIsLast){};

public:
	CXApi* m_pApi;
};

int main_1(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\CTP\\x86\\QuantBox_CTP_Quote.dll";
	char DLLPath2[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\CTP\\x86\\QuantBox_CTP_Trade.dll";
#else
    char DLLPath1[250] = "libQuantBox_CTP_Quote.so";
	char DLLPath2[250] = "libQuantBox_CTP_Trade.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	ServerInfoField				m_ServerInfo2 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.BrokerID, "1017");
	strcpy(m_ServerInfo1.Address, "tcp://ctpmn1-front1.citicsf.com:51213");

	strcpy(m_ServerInfo2.BrokerID, "1017");
	strcpy(m_ServerInfo2.Address, "tcp://ctpmn1-front1.citicsf.com:51205");

	strcpy(m_UserInfo.UserID, "00000025");
	strcpy(m_UserInfo.Password, "123456");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	CXApi* pApi2 = CXApi::CreateApi(DLLPath2);
	if (pApi1 && pApi2)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		if (!pApi2->Init())
		{
			printf("%s\r\n", pApi2->GetLastError());
			pApi2->Disconnect();
			return -1;
		}

		pApi1->RegisterSpi(p);
		pApi2->RegisterSpi(p);

#if defined WINDOWS
        pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
		pApi2->Connect("D:\\", &m_ServerInfo2, &m_UserInfo, 1);
#else
		pApi1->Connect("./", &m_ServerInfo1, &m_UserInfo, 1);
		pApi2->Connect("./", &m_ServerInfo2, &m_UserInfo, 1);
#endif

		getchar();

		pApi1->Subscribe("IF1504", "");

		OrderIDType Out = { 0 };
		OrderField* pOrder = new OrderField();
		memset(pOrder, 0, sizeof(OrderField));
		strcpy(pOrder->InstrumentID, "IF1504");
		pOrder->OpenClose = OpenCloseType::Open;
		pOrder->HedgeFlag = HedgeFlagType::Speculation;
		pOrder->Price = 4000;
		pOrder->Qty = 1;
		pOrder->Type = OrderType::Limit;
		pOrder->Side = OrderSide::Buy;

		pApi2->SendOrder(pOrder, &Out, 1);

		printf("%s\r\n", Out);

		getchar();

		OrderIDType Out2 = { 0 };
		pApi2->CancelOrder(&Out, &Out2, 1);

		getchar();

		pApi1->Disconnect();
		pApi2->Disconnect();
	}

	return 0;
}

int main_2(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\ZeroMQ\\x86\\QuantBox_ZeroMQ_Quote.dll";
#else
	char DLLPath1[250] = "libQuantBox_ZeroMQ_Quote.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.Address, "tcp://127.0.0.1:5555");
	//strcpy(m_ServerInfo1.Address, "pgm://10.10.9.95;239.192.1.1:5555");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	if (pApi1)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		pApi1->RegisterSpi(p);

#if defined WINDOWS || _WIN32
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
#else
		pApi1->Connect("./", &m_ServerInfo1, &m_UserInfo, 1);
#endif

		getchar();

		pApi1->Subscribe("IF1504", "");

		getchar();

		getchar();

		pApi1->Disconnect();
	}

	return 0;
}


int main(int argc, char* argv[])
{
	CXSpiImpl* p = new CXSpiImpl();
#if defined WINDOWS || _WIN32
	char DLLPath1[250] = "C:\\Program Files\\SmartQuant Ltd\\OpenQuant 2014\\XAPI\\CTP\\x86\\QuantBox_CTP_Quote.dll";
#else
	char DLLPath1[250] = "libQuantBox_CTP_Quote.so";
#endif

	ServerInfoField				m_ServerInfo1 = { 0 };
	UserInfoField				m_UserInfo = { 0 };

	strcpy(m_ServerInfo1.BrokerID, "0272");
	strcpy(m_ServerInfo1.Address, "tcp://180.168.146.181:10210");
	m_ServerInfo1.TopicId = 100;// femas这个地方一定不能省
	strcpy(m_ServerInfo1.ExtendInformation, "tcp://*:5555");//这需要对Femas进行改造
	
	strcpy(m_UserInfo.UserID, "00049");
	strcpy(m_UserInfo.Password, "123456");

	CXApi* pApi1 = CXApi::CreateApi(DLLPath1);
	if (pApi1)
	{
		if (!pApi1->Init())
		{
			printf("%s\r\n", pApi1->GetLastError());
			pApi1->Disconnect();
			return -1;
		}

		p->m_pApi = pApi1;
		pApi1->RegisterSpi(p);

#if defined WINDOWS || _WIN32
		pApi1->Connect("D:\\", &m_ServerInfo1, &m_UserInfo, 1);
#else
		pApi1->Connect("./", &m_ServerInfo1, &m_UserInfo, 1);
#endif

		do
		{
			int c = getchar();
			if (c == 'q')
				break;
		} while (true);

		pApi1->Disconnect();
	}

	return 0;
}


