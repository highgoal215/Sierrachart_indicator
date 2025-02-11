#include "sierrachart.h"

SCDLLName("Heikin Ashi RSI with Smoothing")

SCSFExport scsf_HeikinAshiRSI(SCStudyInterfaceRef sc)
{
    
        // Define input parameters
    SCInputRef LengthHARSI = sc.Input[0];
    SCInputRef Smoothing = sc.Input[1];
    SCInputRef Source = sc.Input[2];
    SCInputRef LengthRSI = sc.Input[3];
    SCInputRef ModeRSI = sc.Input[4];
    SCInputRef UpperOB = sc.Input[5];
    SCInputRef UpperOBExtreme = sc.Input[6];
    SCInputRef LowerOS = sc.Input[7];
    SCInputRef LowerOSExtreme = sc.Input[8];
    SCInputRef Length = sc.Input[9];
    SCInputRef ShowZones = sc.Input[10];
    SCInputRef BarSize=sc.Input[11];
    //color define
    COLORREF extra_extreme_buy_color = RGB(255, 0, 60); // #ff003c
    COLORREF extra_extreme_sell_color =RGB(0, 30, 255); // #001eff
    COLORREF sea_zone_upbar_color = RGB(0, 224, 60); // #00e040
    COLORREF sea_zone_downbar_color = RGB(224, 26, 0); // #e01a00
    COLORREF frontier_buy_sweep_color = RGB(0, 0, 0); // #000000
    COLORREF frontier_sell_sweep_color = RGB(0, 0, 0); // #000000
    COLORREF extreme_sell_1_color = RGB(214, 0, 51); // #d60033
    COLORREF extreme_sell_2_color=RGB(214,0,51);
    COLORREF extreme_sell_3_color=RGB(255,255,0);
    COLORREF extreme_sell_4_color=RGB(255,255,255);
    COLORREF extreme_buy_1_color = RGB(255, 0, 255); // #d60033
    COLORREF extreme_buy_2_color=RGB(128,0,128);
    COLORREF extreme_buy_3_color=RGB(0,255,255);
    COLORREF extreme_buy_4_color=RGB(255,255,255);
    COLORREF frontier_sell_color=RGB(33,150,243);
    COLORREF frontier_buy_color=RGB(76,175,80);
    float open_rsi;
    float high_rsi; 
    float low_rsi ;
    float close_rsi;
    if (sc.SetDefaults) {
        sc.GraphName = "Heikin Ashi RSI with Smoothing";
        sc.StudyDescription = "Heikin Ashi RSI with Open Smoothing";
        sc.GraphRegion = 0;
        sc.AutoLoop = 1;

       sc.GraphName = "Edge Zone";
            sc.StudyDescription = "Edge Zone";
            sc.AutoLoop = 1;

            LengthHARSI.Name = "Length HARSI";
            LengthHARSI.SetFloat(14);
            LengthHARSI.SetFloatLimits(1, 100);

            Smoothing.Name = "Open Smoothing";
            Smoothing.SetFloat(1);
            Smoothing.SetFloatLimits(1, 100);

            Source.Name = "Source";
            Source.SetInputDataIndex(SC_HLC_AVG);

            LengthRSI.Name = "Length RSI";
            LengthRSI.SetFloat(7);
            LengthRSI.SetFloatLimits(1, INT_MAX);

            ModeRSI.Name = "Smoothed Mode RSI?";
            ModeRSI.SetYesNo(1);

            UpperOB.Name = "OB";
            UpperOB.SetFloat(20);
            UpperOB.SetFloatLimits(1, 50);

            UpperOBExtreme.Name = "OB Extreme";
            UpperOBExtreme.SetFloat(30);
            UpperOBExtreme.SetFloatLimits(1, 50);

            LowerOS.Name = "OS";
            LowerOS.SetFloat(-20);
            LowerOS.SetFloatLimits(-50, -1);

            LowerOSExtreme.Name = "OS Extreme";
            LowerOSExtreme.SetFloat(-30);
            LowerOSExtreme.SetFloatLimits(-50, -1);

            Length.Name = "Length";
            Length.SetFloat(20);
            Length.SetFloatLimits(1, INT_MAX);

            BarSize.Name="Bar Size";
            BarSize.SetFloat(6);
            BarSize.SetFloatLimits(1,INT_MAX);

        // sc.Subgraph[0].Name = "HA RSI Open";
        // sc.Subgraph[0].DrawStyle = DRAWSTYLE_BAR;
        // sc.Subgraph[0].PrimaryColor = RGB(0, 255, 0);
        sc.Subgraph[5].Name = "Volume";
        sc.Subgraph[5].DrawStyle = DRAWSTYLE_BAR;


        // sc.Subgraph[1].Name = "HA RSI High";
        // sc.Subgraph[1].DrawStyle = DRAWSTYLE_BAR;
        // sc.Subgraph[1].PrimaryColor = RGB(255, 0, 0);

        // sc.Subgraph[2].Name = "HA RSI Low";
        // sc.Subgraph[2].DrawStyle = DRAWSTYLE_BAR;
        // sc.Subgraph[2].PrimaryColor = RGB(0, 0, 255);

        // sc.Subgraph[3].Name = "HA RSI Close";
        // sc.Subgraph[3].DrawStyle = DRAWSTYLE_BAR;
        // sc.Subgraph[3].PrimaryColor = RGB(255, 255, 0);

        return;
    }

    // Persistent arrays for Heikin Ashi calculations
    SCFloatArrayRef ha_open = sc.Subgraph[4];
    SCFloatArrayRef ha_high =  sc.Subgraph[5];
    SCFloatArrayRef ha_low = sc.Subgraph[6];
    SCFloatArrayRef ha_close = sc.Subgraph[7];

    // Persistent arrays for RSI calculations
    SCFloatArrayRef avg_gain = sc.Subgraph[8];
    SCFloatArrayRef avg_loss = sc.Subgraph[9];

    // Persistent arrays for smoothed values
    SCFloatArrayRef smoothed_open = sc.Subgraph[10];

    int index = sc.Index;
    int length = LengthHARSI.GetInt();
    int smoothing = Smoothing.GetInt();

    // Calculate Heikin Ashi values
    if (index == 0) {
        ha_open[index] = (sc.Open[index] + sc.Close[index]) / 2.0f;
        ha_high[index] = sc.High[index];
        ha_low[index] = sc.Low[index];
        ha_close[index] = (sc.Open[index] + sc.High[index] + sc.Low[index] + sc.Close[index]) / 4.0f;
    } else {
        ha_open[index] = (ha_open[index - 1] + ha_close[index - 1]) / 2.0f;
        ha_high[index] = fmax(sc.High[index], fmax(ha_open[index], ha_close[index]));
        ha_low[index] = fmin(sc.Low[index], fmin(ha_open[index], ha_close[index]));
        ha_close[index] = (sc.Open[index] + sc.High[index] + sc.Low[index] + sc.Close[index]) / 4.0f;
    }

    // Calculate RSI components
    if (index < 1) {
        avg_gain[index] = 0;
        avg_loss[index] = 0;
        smoothed_open[index] = 0;
        sc.Subgraph[0][index] = 0;
        sc.Subgraph[1][index] = 0;
        sc.Subgraph[2][index] = 0;
        sc.Subgraph[3][index] = 0;
        return;
    }

    float change = ha_close[index] - ha_close[index - 1];
    float gain = change > 0 ? change : 0;
    float loss = change < 0 ? -change : 0;

    if (index < length) {
        // Accumulation phase
        avg_gain[index] = avg_gain[index - 1] + gain;
        avg_loss[index] = avg_loss[index - 1] + loss;
        smoothed_open[index] = 0;
        sc.Subgraph[0][index] = 0;
        sc.Subgraph[1][index] = 0;
        sc.Subgraph[2][index] = 0;
        sc.Subgraph[3][index] = 0;
    } else if (index == length) {
        // Initial average calculation
        avg_gain[index] = (avg_gain[index - 1] + gain) / length;
        avg_loss[index] = (avg_loss[index - 1] + loss) / length;

        float rs = avg_loss[index] != 0 ? avg_gain[index] / avg_loss[index] : 0;
        float rsi = 100.0f - (100.0f / (1.0f + rs));
        float zrsi = rsi - 50.0f;

        smoothed_open[index] = zrsi;
        sc.Subgraph[0][index] = smoothed_open[index];
        sc.Subgraph[1][index] = zrsi;
        sc.Subgraph[2][index] = zrsi;
        sc.Subgraph[3][index] = zrsi;
    } else {
        // Smoothing with Wilder's method
        avg_gain[index] = (avg_gain[index - 1] * (length - 1) + gain) / length;
        avg_loss[index] = (avg_loss[index - 1] * (length - 1) + loss) / length;

        float rs = avg_loss[index] != 0 ? avg_gain[index] / avg_loss[index] : 0;
        float rsi = 100.0f - (100.0f / (1.0f + rs));
        float zrsi = rsi - 50.0f;

        // Calculate smoothed open
        if (index < smoothing) {
            smoothed_open[index] = (smoothed_open[index - 1] + zrsi) / 2.0f;
        } else {
            smoothed_open[index] = (smoothed_open[index - 1] * smoothing + zrsi) / (smoothing + 1);
        }

        // Calculate Heikin Ashi RSI values
         open_rsi = smoothed_open[index];
         high_rsi = fmax(zrsi, fmax(open_rsi, zrsi));
         low_rsi = fmin(zrsi, fmin(open_rsi, zrsi));
         close_rsi = (open_rsi + high_rsi + low_rsi + zrsi) / 4.0f;
        SCString message;
        message.Format("closeRSI: %.5f", close_rsi);
        sc.AddMessageToLog(message, 1);

        sc.Subgraph[0][index] = open_rsi;
        sc.Subgraph[1][index] = high_rsi;
        sc.Subgraph[2][index] = low_rsi;
        sc.Subgraph[3][index] = close_rsi;
    }

    return;
    float open=open_rsi;
    float high=high_rsi; 
    float low=low_rsi ;
    float close=close_rsi;
    
    // Main zones
    bool extreme_sell = close > UpperOBExtreme.GetFloat();
    bool frontier_sell = close < UpperOBExtreme.GetFloat() && close > UpperOB.GetFloat();
    bool sea_zone = close < UpperOB.GetFloat() && close > LowerOS.GetFloat();
    bool frontier_buy = close < LowerOS.GetFloat() && close > LowerOSExtreme.GetFloat();
    bool extreme_buy = close < LowerOSExtreme.GetFloat();

    bool fr_buy_sweep = sea_zone && high > UpperOB.GetFloat();
    bool fr_sel_sweep = sea_zone && low < LowerOS.GetFloat();
    bool extreme_sell_1 = close > 27.5 && close <= 30;
    bool extreme_sell_2 = close > 25 && close <= 27.5;
    bool extreme_sell_3 = close > 22.5 && close <= 25;
    bool extreme_sell_4 = close > 20 && close <= 22.5;
    bool extreme_buy_1 = close < -27.5 && close >= -30;
    bool extreme_buy_2 = close < -25 && close >= -27.5;
    bool extreme_buy_3 = close < -22.5 && close >= -25;
    bool extreme_buy_4 = close < -20 && close >= -22.5;
    // Extra extreme zones
    bool extra_extreme_sell = close > 30;
    bool extra_extreme_buy = close < -30;

    COLORREF candle_color = RGB(128, 128, 128); // Default to gray
    if (extra_extreme_sell)
            candle_color = extra_extreme_sell_color;
    else if (extreme_sell_1)
        candle_color = extreme_sell_1_color;
    else if (extreme_sell_2)
        candle_color = extreme_sell_2_color; // Orange
    else if (extreme_sell_3)
        candle_color = extreme_sell_3_color; // Yellow
    else if (extreme_sell_4)
        candle_color = extreme_sell_4_color; // White
    else if (fr_sel_sweep)
        candle_color = frontier_sell_sweep_color;
    else if (frontier_sell)
            candle_color=frontier_sell_color;
    else if (extra_extreme_buy)
        candle_color = extra_extreme_buy_color;
    else if (extreme_buy_1)
        candle_color = extreme_buy_1_color; // Fuchsia
    else if (extreme_buy_2)
        candle_color = extreme_buy_2_color; // Purple
    else if (extreme_buy_3)
        candle_color =extreme_buy_3_color; // Aqua
    else if (extreme_buy_4)
        candle_color = extreme_buy_4_color; // White
    else if (fr_buy_sweep)
        candle_color = frontier_buy_sweep_color;
    else if (frontier_buy)
        candle_color=frontier_buy_color;
    else  (sc.Close[sc.Index]>sc.Open[sc.Index]) ?
    candle_color=sea_zone_upbar_color: candle_color=sea_zone_downbar_color;
            // Plot CNV with determined color
    sc.Subgraph[5].DataColor[sc.Index] = candle_color;
    sc.Subgraph[5].LineWidth=12;
    sc.Subgraph[5][sc.Index] =sc.Volume[sc.Index];
}