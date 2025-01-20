#include "sierrachart.h"

SCDLLName("Zone Colored Candles")

// Inputs
SCSFExport scsf_ZoneColoredCandles(SCStudyInterfaceRef sc)
{
	SCInputRef Input_Length = sc.Input[0];
    SCInputRef Input_Smoothing = sc.Input[1];

    SCInputRef Input_OB = sc.Input[2];
    SCInputRef Input_OBExtreme = sc.Input[3];
    SCInputRef Input_OS = sc.Input[4];
    SCInputRef Input_OSExtreme = sc.Input[5];

    if (sc.SetDefaults) {
        sc.GraphName = "Zone Colored Candles";
        sc.StudyDescription = "Zone Colored Candles similar to the TradingView indicator";

        Input_Length.Name = "Length";
        Input_Length.SetInt(14);
        Input_Length.SetIntLimits(1, INT_MAX);

        Input_Smoothing.Name = "Open Smoothing";
        Input_Smoothing.SetInt(1);
        Input_Smoothing.SetIntLimits(1, 100);

        Input_OB.Name = "OB";
        Input_OB.SetInt(20);
        Input_OB.SetIntLimits(1, 50);

        Input_OBExtreme.Name = "OB Extreme";
        Input_OBExtreme.SetInt(30);
        Input_OBExtreme.SetIntLimits(1, 50);

        Input_OS.Name = "OS";
        Input_OS.SetInt(-20);
        Input_OS.SetIntLimits(-50, -1);

        Input_OSExtreme.Name = "OS Extreme";
        Input_OSExtreme.SetInt(-30);
        Input_OSExtreme.SetIntLimits(-50, -1);

        sc.AutoLoop = 1;

        return;
    }

    int length = Input_Length.GetInt();
    int smoothing = Input_Smoothing.GetInt();
    int upper = Input_OB.GetInt();
    int upperExtreme = Input_OBExtreme.GetInt();
    int lower = Input_OS.GetInt();
    int lowerExtreme = Input_OSExtreme.GetInt();

    float ha_open = (sc.BaseData[SC_OPEN][sc.Index] + sc.BaseData[SC_CLOSE][sc.Index]) / 2;
    float ha_close = (sc.BaseData[SC_OPEN][sc.Index] + sc.BaseData[SC_HIGH][sc.Index] + sc.BaseData[SC_LOW][sc.Index] + sc.BaseData[SC_CLOSE][sc.Index]) / 4;
    float ha_high = max(max(sc.BaseData[SC_HIGH][sc.Index], ha_open), ha_close);
    float ha_low = min(min(sc.BaseData[SC_LOW][sc.Index], ha_open), ha_close);

    float ha_rsi = sc.RSI(sc.BaseDataIn[SC_LAST], sc.Index, length);
    float adjusted_ha_rsi = ha_rsi - 50;

    COLORREF candleColor = RGB(255, 255, 255);

    if (adjusted_ha_rsi > 30) {
        candleColor = RGB(255, 0, 60);
    } else if (adjusted_ha_rsi < -30) {
        candleColor = RGB(0, 0, 255);
    } 

    SCFloatArrayRef openArray = sc.Subgraph[0].Data;
    SCFloatArrayRef highArray = sc.Subgraph[1].Data;
    SCFloatArrayRef lowArray = sc.Subgraph[2].Data;
    SCFloatArrayRef closeArray = sc.Subgraph[3].Data;

    openArray[sc.Index] = ha_open;
    highArray[sc.Index] = ha_high;
    lowArray[sc.Index] = ha_low;
    closeArray[sc.Index] = ha_close;

    sc.Subgraph[0].DataColor[sc.Index] = candleColor;
    sc.Subgraph[1].DataColor[sc.Index] = candleColor;
    sc.Subgraph[2].DataColor[sc.Index] = candleColor;
    sc.Subgraph[3].DataColor[sc.Index] = candleColor;
}