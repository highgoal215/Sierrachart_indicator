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
	SCInputRef Input_Colorextra_extreme_sell = sc.Input[6];
	SCInputRef Input_Colorextra_extreme_buy = sc.Input[7];
	SCInputRef Input_Colorsea_zone_upbar = sc.Input[8];
	SCInputRef Input_Colorsea_zone_downbar = sc.Input[9];
	SCInputRef Input_Colorfrontier_buy_sweep = sc.Input[10];
	SCInputRef Input_Colorfrontier_sell_sweep = sc.Input[11];
	SCInputRef Input_Colorextreme_sell_1 = sc.Input[12];
	SCInputRef Input_Colorextreme_sell_2 = sc.Input[13];
	SCInputRef Input_Colorextreme_sell_3 = sc.Input[14];
	SCInputRef Input_Colorextreme_sell_4 = sc.Input[15];
	SCInputRef Input_Colorfrontier_sell = sc.Input[16];
	SCInputRef Input_Colorextreme_buy_1 = sc.Input[17];
	SCInputRef Input_Colorextreme_buy_2 = sc.Input[18];
	SCInputRef Input_Colorextreme_buy_3 = sc.Input[19];
	SCInputRef Input_Colorextreme_buy_4 = sc.Input[20];
	SCInputRef Input_Colorfrontier_buy = sc.Input[21];
    if (sc.SetDefaults) {
        sc.GraphName = "Zone Colored Candles";
        sc.StudyDescription = "Zone Colored Candles similar to the TradingView indicator";

        Input_Length.Name = "Length";
        Input_Length.SetInt(14);
        Input_Length.SetIntLimits(1, INT_MAX);

        Input_Smoothing.Name = "Open Smoothing";
        Input_Smoothing.SetInt(7);
        Input_Smoothing.SetIntLimits(1, 100);

        Input_OB.Name = "OB Upper";
        Input_OB.SetInt(20);
        Input_OB.SetIntLimits(1, 50);

        Input_OBExtreme.Name = "OB Extreme";
        Input_OBExtreme.SetInt(30);
        Input_OBExtreme.SetIntLimits(1, 50);

        Input_OS.Name = "OS Lower";
        Input_OS.SetInt(-20);
        Input_OS.SetIntLimits(-50, -1);

        Input_OSExtreme.Name = "OS Extreme";
        Input_OSExtreme.SetInt(-30);
        Input_OSExtreme.SetIntLimits(-50, -1);

		Input_Colorextra_extreme_sell.Name = "extra_extreme_sell";
		Input_Colorextra_extreme_sell.SetColor(RGB(0, 30, 255));
		
		Input_Colorextra_extreme_buy.Name = "extra_extreme_buy";
		Input_Colorextra_extreme_buy.SetColor(RGB(255, 0, 60));
		
		Input_Colorsea_zone_upbar.Name = "sea_zone_upbar";
		Input_Colorsea_zone_upbar.SetColor(RGB(0, 224,60));
		
		Input_Colorsea_zone_downbar.Name = "sea_zone_downbar";
		Input_Colorsea_zone_downbar.SetColor(RGB(224, 26, 0));
		
		Input_Colorfrontier_buy_sweep.Name = "frontier_buy_sweep";
		Input_Colorfrontier_buy_sweep.SetColor(RGB(0, 0, 0));
		
		Input_Colorfrontier_sell_sweep.Name = "frontier_sell_sweep";
		Input_Colorfrontier_sell_sweep.SetColor(RGB(0, 0, 0));
		
		Input_Colorextreme_sell_1.Name = "extreme_sell_1";
		Input_Colorextreme_sell_1.SetColor(RGB(214, 0, 51));
		
		Input_Colorextreme_sell_2.Name = "extreme_sell_2";
		Input_Colorextreme_sell_2.SetColor(RGB(255, 165, 0));
		
		Input_Colorextreme_sell_3.Name = "extreme_sell_3";
		Input_Colorextreme_sell_3.SetColor(RGB(255, 255, 0));
		
		Input_Colorextreme_sell_4.Name = "extreme_sell_4";
		Input_Colorextreme_sell_4.SetColor(RGB(255, 255, 255));
		
		Input_Colorfrontier_sell.Name = "Frontier_sell";
		Input_Colorfrontier_sell.SetColor(RGB(0, 0, 255));
		
		Input_Colorextreme_buy_1.Name = "extreme_buy_1";
		Input_Colorextreme_buy_1.SetColor(RGB(255, 0, 255));
		
		Input_Colorextreme_buy_2.Name = "extreme_buy_2";
		Input_Colorextreme_buy_2.SetColor(RGB(128, 0, 128));
		
		Input_Colorextreme_buy_3.Name = "extreme_buy_3";
		Input_Colorextreme_buy_3.SetColor(RGB(0, 255, 255));
		
		Input_Colorextreme_buy_4.Name = "extreme_buy_4";
		Input_Colorextreme_buy_4.SetColor(RGB(255, 255, 255));
		
		Input_Colorfrontier_buy.Name = "frontier_buy";
		Input_Colorfrontier_buy.SetColor(RGB(0, 255, 0));
        sc.AutoLoop = 1;
		sc.GraphRegion = 1;
        return;
    }
	
    int length = Input_Length.GetInt();
    int smoothing = Input_Smoothing.GetInt();
    int upper = Input_OB.GetInt();
    int upperExtreme = Input_OBExtreme.GetInt();
    int lower = Input_OS.GetInt();
	int lowerExtreme = Input_OSExtreme.GetInt();
	
	// Declare the Base Data reference
    //SCBaseDataRef BaseDataIn = sc.BaseData;

    // Declare subgraphs for Heikin Ashi Open, High, Low, Close
    SCSubgraphRef HeikinAshiOut = sc.Subgraph[0]; // Open prices
    HeikinAshiOut.Name = "HA Open";
    HeikinAshiOut.Arrays[0] = sc.Subgraph[1]; // High prices
    HeikinAshiOut.Arrays[1] = sc.Subgraph[2]; // Low prices
    HeikinAshiOut.Arrays[2] = sc.Subgraph[3]; // Close prices

    // Call the HeikinAshi function
    // Here we can use the auto-looping feature, so Index is handled automatically
    sc.HeikinAshi(sc.BaseDataIn, HeikinAshiOut, sc.Index, sc.ArraySize, 0); // SetCloseToCurrentPriceAtLastBar is set to 0
    // Access the Heikin Ashi values
	SCFloatArrayRef haOpen = HeikinAshiOut.Data;
	SCFloatArrayRef haHigh = HeikinAshiOut.Arrays[0];
	SCFloatArrayRef haLow = HeikinAshiOut.Arrays[1];
	SCFloatArrayRef haClose = HeikinAshiOut.Arrays[2];
	
	SCString message;
	message.Format("HA Open: %.5f", sc.Open[sc.Index]);
	sc.AddMessageToLog(message, 1);
	message.Format("HA High: %.5f", sc.High[sc.Index]);
	sc.AddMessageToLog(message, 1);
	message.Format("HA Low: %.5f", sc.Low[sc.Index]);
	sc.AddMessageToLog(message, 1);
	message.Format("HA Cloe: %.5f", sc.Close[sc.Index]);
	sc.AddMessageToLog(message, 1);
	SCSubgraphRef rsiClose = sc.Subgraph[4];
	float openRSI = 0.0f;
	float highRSI = 0.0f;
	float lowRSI = 0.0f;
	float closeRSI = 0.0f;
	
	sc.RSI(haClose, rsiClose, MOVAVGTYPE_SIMPLE, length);
	
	closeRSI = rsiClose[sc.Index] - 50.0f;
	openRSI = (sc.Index > 0) ? rsiClose[sc.Index - 1] - 50.0f : closeRSI;
	
	SCSubgraphRef rsiHigh = sc.Subgraph[5];
	sc.RSI(haHigh, rsiHigh, MOVAVGTYPE_SIMPLE, length);
	
	SCSubgraphRef rsiLow = sc.Subgraph[6];
	sc.RSI(haLow, rsiLow, MOVAVGTYPE_SIMPLE, length);
	
	float highRSIRaw = rsiHigh[sc.Index] - 50.0f;
    float lowRSIRaw = rsiLow[sc.Index] - 50.0f;
	
	highRSI = max(highRSIRaw, lowRSIRaw);
	lowRSI = min(highRSIRaw, lowRSIRaw);
	
	//Calculate open, high, low, close values
	SCFloatArrayRef close = sc.Subgraph[7].Data;
	SCFloatArrayRef open = sc.Subgraph[8].Data;
	SCFloatArrayRef high = sc.Subgraph[9].Data;
	SCFloatArrayRef low = sc.Subgraph[10].Data;
	
	close[sc.Index] = (openRSI + highRSI + lowRSI + closeRSI) / 4.0f;
	open[sc.Index] = std::isnan(open[smoothing]) ? (openRSI + closeRSI) / 2.0f : (open[1] * smoothing + close[1]) / (smoothing + 1); 
	high[sc.Index] = max(highRSI, max(open[sc.Index], close[sc.Index]));
	low[sc.Index] = min(lowRSI, min(open[sc.Index], close[sc.Index]));
	message.Format("close---->: %.5f", close[sc.Index]);
	sc.AddMessageToLog(message, 1);
	// Define colors
    int extra_extreme_buy_color = RGB(255, 0, 60);   // #ff003c
    int extra_extreme_sell_color = RGB(0, 30, 255);   // #001eff
    int sea_zone_upbar_color = RGB(0, 224, 64);       // #00e040
    int sea_zone_downbar_color = RGB(224, 26, 0);     // #e01a00
    int frontier_buy_sweep_color = RGB(0, 0, 0);       // #000000
    int frontier_sell_sweep_color = RGB(0, 0, 0);      // #000000
	int extreme_sell_1_color = RGB(214, 0, 51);
	
	// Define zones
    bool extra_extreme_sell = close[sc.Index] > 30;
    bool extreme_sell_1 = close[sc.Index] > 27.5f && close[sc.Index] <= 30;
    bool extreme_sell_2 = close[sc.Index] > 25 && close[sc.Index] <= 27.5f;
    bool extreme_sell_3 = close[sc.Index] > 22.5f && close[sc.Index] <= 25;
    bool extreme_sell_4 = close[sc.Index] > 20 && close[sc.Index] <= 22.5f;
    bool frontier_sell = close[sc.Index] < upperExtreme && close[sc.Index] > upper;
    bool sea_zone = close[sc.Index] < upper && close[sc.Index] > lower;
    bool frontier_buy = close[sc.Index] < lower && close[sc.Index] > lowerExtreme;
    bool extreme_buy_4 = close[sc.Index] < -20 && close[sc.Index] >= -22.5f;
    bool extreme_buy_3 = close[sc.Index] < -22.5f && close[sc.Index] >= -25;
    bool extreme_buy_2 = close[sc.Index] < -25 && close[sc.Index] >= -27.5f;
    bool extreme_buy_1 = close[sc.Index] < -27.5f && close[sc.Index] >= -30;
    bool extra_extreme_buy = close[sc.Index] < -30;
    bool fr_buy_sweep = (sea_zone && high[sc.Index] > upper);
    bool fr_sel_sweep = (sea_zone && low[sc.Index] < lower);
	
	// Determine candle color
    int candle_color;

    if (extra_extreme_sell)
        candle_color = Input_Colorextra_extreme_sell.GetInt();
    else if (extreme_sell_1)
        candle_color = Input_Colorextreme_sell_1.GetInt();
    else if (extreme_sell_2)
        candle_color = Input_Colorextreme_sell_2.GetInt();
    else if (extreme_sell_3)
        candle_color = Input_Colorextreme_sell_3.GetInt();
    else if (extreme_sell_4)
        candle_color = Input_Colorextreme_sell_4.GetInt();
    else if (frontier_sell)
        candle_color = Input_Colorfrontier_sell.GetInt();
    else if (extra_extreme_buy)
        candle_color = Input_Colorextra_extreme_buy.GetInt();
    else if (extreme_buy_1)
        candle_color = Input_Colorextreme_buy_1.GetInt();
    else if (extreme_buy_2)
        candle_color = Input_Colorextreme_buy_2.GetInt();
    else if (extreme_buy_3)
        candle_color = Input_Colorextreme_buy_3.GetInt();
    else if (extreme_buy_4)
        candle_color = Input_Colorextreme_buy_4.GetInt();
    else if (frontier_buy)
        candle_color = Input_Colorfrontier_buy.GetInt();
    else if (fr_buy_sweep)
        candle_color = Input_Colorfrontier_buy_sweep.GetInt();
    else if (fr_sel_sweep)
        candle_color = Input_Colorfrontier_sell_sweep.GetInt();
    else
        candle_color = (sc.Close[sc.Index] > sc.Open[sc.Index]) ? Input_Colorsea_zone_upbar.GetInt() : Input_Colorsea_zone_downbar.GetInt();
	
	// Define line properties
    COLORREF upperExtremeColor = RGB(0, 0, 255); // Blue color
    COLORREF upperColor = RGB(0, 0, 255);        // Blue color
    COLORREF lowerColor = RGB(255, 0, 0);        // Red color
    COLORREF lowerExtremeColor = RGB(255, 0, 0); // Red color

    unsigned short lineWidth = 1; // Line width
    unsigned short lineStyle = LINESTYLE_SOLID; // Solid line style
    int drawValueLabel = 1; // Draw value label
    int drawNameLabel = 1; // Do not draw name label
	
	sc.Subgraph[0][sc.Index] = sc.Close[sc.Index];
	sc.Subgraph[0].DrawStyle = DRAWSTYLE_COLOR_BAR;	
    sc.Subgraph[0].SecondaryColorUsed = 1;
	sc.Subgraph[0].DataColor[sc.Index] = candle_color;
}



