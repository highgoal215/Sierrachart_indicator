#include "sierrachart.h"
SCDLLName("Net VV2")
float smothingget;
// Function to calculate zero-centered RSI
float f_zrsi(SCFloatArrayRef source, float length, SCStudyInterfaceRef sc)
{   
    SCString message;
    message.Format("Source  :%.5f", source);
    sc.AddMessageToLog(message,1);
    message.Format("length  :%.5f", length);
    sc.AddMessageToLog(message,1);
    SCSubgraphRef RSIArray = sc.Subgraph[5]; // Assuming Subgraph[5] is used for RSI values
    sc.RSI(source, RSIArray, sc.Index, length);
    message.Format("RSI  :%.5f", RSIArray[sc.Index]-50);
    sc.AddMessageToLog(message,1);
    return RSIArray[sc.Index] - 50;

}
// Function to calculate smoothed RSI
float f_rsi(SCFloatArrayRef source, float length, bool mode, SCStudyInterfaceRef sc)
{
    float zrsi = f_zrsi(source, length, sc);
    static float smoothed = 0;
    if (sc.Index == 0)
        smoothed = zrsi;
    else
        smoothed = (smoothed + zrsi) / 2;
    return mode ? smoothed : zrsi;
}
// Function to calculate Heikin Ashi RSI
void f_rsiHeikinAshi(float length, SCStudyInterfaceRef sc, float* open, float* high, float* low, float* close)
{
    SCString message;
    SCFloatArrayRef haOpen = sc.Subgraph[6].Data; // Assuming Subgraph[6] is used for Heikin Ashi open values
    SCFloatArrayRef haHigh = sc.Subgraph[7].Data; // Assuming Subgraph[7] is used for Heikin Ashi high values
    SCFloatArrayRef haLow = sc.Subgraph[8].Data; // Assuming Subgraph[8] is used for Heikin Ashi low values
    SCFloatArrayRef haClose = sc.Subgraph[9].Data; // Assuming Subgraph[9] is used for Heikin Ashi close values
    haOpen[sc.Index] = (sc.Open[sc.Index ] + sc.Close[sc.Index]) / 2;
    haClose[sc.Index] = (sc.Open[sc.Index] + sc.High[sc.Index] + sc.Low[sc.Index] + sc.Close[sc.Index]) / 4;
    haHigh[sc.Index] = max(sc.High[sc.Index], max(haOpen[sc.Index], haClose[sc.Index]));
    haLow[sc.Index] = min(sc.Low[sc.Index], min(haOpen[sc.Index], haClose[sc.Index]));
    sc.Subgraph[15].Data[sc.Index]=haOpen[sc.Index];
    sc.Subgraph[16].Data[sc.Index]=haClose[sc.Index];
    message.Format("haclose:%.5f",haClose);
    sc.AddMessageToLog(message,1);
    message.Format("subgtaph[16] :%.5f", sc.Subgraph[16].Data);
    sc.AddMessageToLog(message,1);
    message.Format(" current low:%.5f",sc.Low[sc.Index]);
    sc.AddMessageToLog(message,1);
    message.Format("current halow :%.5f",haLow[sc.Index]);
    sc.AddMessageToLog(message,1);
    float closeRSI = f_zrsi(sc.Subgraph[16].Data, length, sc);
    float openRSI = closeRSI;
    float highRSI_raw = f_zrsi(haHigh, length, sc);
    float lowRSI_raw = f_zrsi(haLow, length, sc);
    float highRSI = max(highRSI_raw, lowRSI_raw);
    float lowRSI = min(highRSI_raw, lowRSI_raw);
    *close = (openRSI + highRSI + lowRSI + closeRSI) / 4;

    static float prev_open = 0;
    if (sc.Index == 0)
    {
        *open = (openRSI + closeRSI) / 2;
    }
    else
    {
        *open = (prev_open * smothingget+ (*close)) / (smothingget + 1);
    }
    prev_open = *open;
    *high = max(highRSI, max((*open), (*close)));
    *low = min(lowRSI, min((*open), (*close)));
       message.Format("*close------>  %.5f",*close);
    sc.AddMessageToLog(message,1);

}

SCSFExport scsf_NetVolumeCalculation(SCStudyInterfaceRef sc)
{
    SCString message;
   
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
    SCInputRef BarSise=sc.Input[11];
    SCInputRef Bullishcolor=sc.Input[12];
    SCInputRef BearishColor=sc.Input[13];
    COLORREF backgroundcolor= RGB(128, 128, 128); //defalut background color
         // Color definitions
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

    // // Define colors for shapes
    // COLORREF color_red = RGB(255, 0, 0);          // Red for extreme sell shapes
    // COLORREF color_orange = RGB(255, 165, 0);     // Orange for extreme sell shapes
    // COLORREF color_yellow = RGB(255, 255, 0);      // Yellow for extreme sell shapes
    // COLORREF color_white = RGB(255, 255, 255);     // White for shapes
    // COLORREF color_purple = RGB(128, 0, 128);      // Purple for extreme buy shapes
    // COLORREF color_fuchsia = RGB(255, 0, 255);     // Fuchsia for extreme buy shapes
    // COLORREF color_aqua = RGB(0, 255, 255);        // Aqua for extreme buy shapes
     // Color definition
    // COLORREF frontier_buy_sweep_color =  RGB(128, 128, 128); //rgb(51, 49, 49)
    // COLORREF frontier_sell_sweep_color =  RGB(128, 128, 128); //rgb(51, 49, 49)

    // Set default input values
    if (sc.SetDefaults)
    {
       
        sc.GraphName = "Net Volume of Positive and Negative Volume VV2";
        sc.StudyDescription = "Net Volume of Positive and Negative Volume VV2 with Heikin Ashi and RSI calculations";
        sc.AutoLoop = 1;

        LengthHARSI.Name = "Length HARSI";
        LengthHARSI.SetFloat(14);
        LengthHARSI.SetFloatLimits(1, INT_MAX);

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
        BarSise.Name="Bar Size";
        BarSise.SetFloat(12);
        BarSise.SetFloatLimits(1,INT_MAX);

        ShowZones.Name = "Show Bullish/Bearish Zones";
        ShowZones.SetYesNo(true);
        sc.Subgraph[0].Name = "Bullish Zone";
        sc.Subgraph[1].Name = "Bearish Zone";
        sc.Subgraph[2].Name = "Neutral Zone";
        sc.Subgraph[0].DrawStyle=DRAWSTYLE_BACKGROUND;
        sc.Subgraph[1].DrawStyle=DRAWSTYLE_BACKGROUND;
        sc.Subgraph[2].DrawStyle=DRAWSTYLE_BACKGROUND;
        sc.Subgraph[0].PrimaryColor = RGB(33, 150, 243); // Blue for bullish
        sc.Subgraph[1].PrimaryColor = RGB(255, 82, 82); // Red for bearish
        sc.Subgraph[2].PrimaryColor = RGB(120, 123, 134); // Gray for neutral
        sc.Subgraph[4].Name = "CNV_TB";
        sc.Subgraph[4].DrawStyle = DRAWSTYLE_BAR;
        sc.Subgraph[4].PrimaryColor =  RGB(120, 123, 134);
        sc.Subgraph[6].Name="haopen"; // Assuming Subgraph[6] is used for Heikin Ashi open values
        sc.Subgraph[7].Name="haHigh"; // Assuming Subgraph[7] is used for Heikin Ashi high values
        sc.Subgraph[8].Name="halow"; // Assuming Subgraph[8] is used for Heikin Ashi low values
        sc.Subgraph[9].Name="haClose"; // Assuming Subgraph[9] is used for Heikin Ashi close values
        Bullishcolor.Name="Bullishcolor";
        Bullishcolor.SetColor(sc.Subgraph[0].PrimaryColor);

        BearishColor.Name="BearishColor";
        BearishColor.SetColor(sc.Subgraph[1].PrimaryColor);
        return;
    }

    smothingget=Smoothing.GetFloat();
    float open=0.0f;
    float high=0.0f; 
    float low=0.0f ;
    float close=0.0f;
    message.Format("LengthHARSI.GetFloat()------>: %.5f",LengthHARSI.GetFloat());
    sc.AddMessageToLog(message,1);
    f_rsiHeikinAshi(LengthHARSI.GetFloat(), sc, &open, &high, &low, &close);
        // Determine line color
    message.Format("close :%.5f",close);
    sc.AddMessageToLog(message,1);
    message.Format("hifh : %.5f",high);
    sc.AddMessageToLog(message,1);
    message.Format("low++++++++++++++++++>: %.5f",low);
    sc.AddMessageToLog(message,1);
      // Main zones
    bool extreme_sell = close > UpperOBExtreme.GetFloat();
    bool frontier_sell = close < UpperOBExtreme.GetFloat() && close > UpperOB.GetFloat();
    bool sea_zone = close < UpperOB.GetFloat() && close > LowerOS.GetFloat();
    bool frontier_buy = close < LowerOS.GetFloat() && close > LowerOSExtreme.GetFloat();
    bool extreme_buy = close < LowerOSExtreme.GetFloat();

    bool fr_buy_sweep = sea_zone && sc.High[sc.Index] > UpperOB.GetFloat();
    bool fr_sel_sweep = sea_zone && sc.Low[sc.Index] < LowerOS.GetFloat();
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

  
    float nv=0;
    float change_1 = sc.Close[sc.Index] - sc.Close[sc.Index - 1];
    nv = change_1 > 0 ? sc.Volume[sc.Index] : change_1 < 0 ? -sc.Volume[sc.Index] : 0;  
    SCSubgraphRef  cnvArray = sc.Subgraph[3]; // Temporary array for CNV values
    SCSubgraphRef  smaArray = sc.Subgraph[4]; // Temporary array for SMA values
    cnvArray[sc.Index] =cnvArray[sc.Index-1] +nv;
    // smaArray[sc.Index]=close;
    sc.SimpleMovAvg(cnvArray, smaArray, sc.Index, Length.GetFloat());
    static float cnv_tb =0;
    cnv_tb= cnvArray[sc.Index]-smaArray[sc.Index];
    bool bullishRule = cnv_tb > 0;
    bool bearishRule = cnv_tb <= 0;
    static int ruleState = 0;
    if (bullishRule)
        ruleState = 1;
    else if (bearishRule)
        ruleState = -1;
    else
        ruleState = ruleState;
  if (ShowZones.GetYesNo())
    {   
        if (ruleState == 1)
           {
            sc.Subgraph[0].Data[sc.Index] = 1; // Mark bullish zone
            sc.Subgraph[1].Data[sc.Index] = 0; // No bearish zone
            sc.Subgraph[2].Data[sc.Index] = 0; // No neutral zone
            sc.Subgraph[0].PrimaryColor = Bullishcolor.GetColor(); // Blue for bullish

           }
        else if (ruleState == -1)
           {
            sc.Subgraph[0].Data[sc.Index] = 0; // No bullish zone
            sc.Subgraph[1].Data[sc.Index] = 1; // Mark bearish zone
            sc.Subgraph[2].Data[sc.Index] = 0; // No neutral zone
            sc.Subgraph[1].PrimaryColor = BearishColor.GetColor();
           }
        else
           {
            sc.Subgraph[0].Data[sc.Index] = 0; // No bullish zone
            sc.Subgraph[1].Data[sc.Index] = 0; // No bearish zone
            sc.Subgraph[2].Data[sc.Index] = 1; // Mark neutral zone
            sc.Subgraph[2].PrimaryColor = RGB(128, 128, 128); // Gray for neutral
           }
    }

    // Alert conditions based on crossover and crossunder of CNV threshold with zero line
    bool upp = (cnv_tb > 0 && sc.Index > 0 && cnvArray[sc.Index-1] <= 0);
    bool downn = (cnv_tb <= 0 && sc.Index > 0 && cnvArray[sc.Index-1] > 0);
    // Add alerts based on conditions
   if (upp)
       {
        message.Format("Buy Signal");
	    sc.AddMessageToLog(message, 1);
       }
   if (downn)
       {
        message.Format("Sell Signal");
	    sc.AddMessageToLog(message, 1);
       }
     COLORREF lineColor = RGB(128, 128, 128); // Default to gray
    if (cnv_tb > 0)
    {
        if (extra_extreme_sell)
            lineColor = extra_extreme_sell_color;
        else if (extreme_sell_1)
            lineColor = extreme_sell_1_color;
        else if (extreme_sell_2)
            lineColor = extreme_sell_2_color; // Orange
        else if (extreme_sell_3)
            lineColor = extreme_sell_3_color; // Yellow
        else if (extreme_sell_4)
            lineColor = extreme_sell_4_color; // White
        else if (fr_buy_sweep)
            lineColor = frontier_buy_sweep_color;
    }
    else
    {
        if (extra_extreme_buy)
            lineColor = extra_extreme_buy_color;
        else if (extreme_buy_1)#include "sierrachart.h"
SCDLLName("Net VV2")
float smothingget;
// Function to calculate zero-centered RSI
float f_zrsi(SCFloatArrayRef source, float length, SCStudyInterfaceRef sc)
{   
    SCString message;
    SCSubgraphRef RSIArray = sc.Subgraph[5]; // Assuming Subgraph[5] is used for RSI values
    sc.RSI(source, RSIArray, sc.Index, length);
    message.Format("RSI  :%.5f", RSIArray[sc.Index]-50);
    sc.AddMessageToLog(message,1);
    return RSIArray[sc.Index]-50 ;

}
// Function to calculate smoothed RSI
float f_rsi(SCFloatArrayRef source, float length, bool mode, SCStudyInterfaceRef sc)
{
    float zrsi = f_zrsi(source, length, sc);
    static float smoothed = 0;
    if (sc.Index == 0)
        smoothed = zrsi;
    else
        smoothed = (smoothed + zrsi) / 2;
    return mode ? smoothed : zrsi;
}
// Function to calculate Heikin Ashi RSI
void f_rsiHeikinAshi(float length, SCStudyInterfaceRef sc, float* open, float* high, float* low, float* close)
{
    SCString message;
    SCFloatArrayRef haOpen = sc.Subgraph[6].Data; // Assuming Subgraph[6] is used for Heikin Ashi open values
    SCFloatArrayRef haHigh = sc.Subgraph[7].Data; // Assuming Subgraph[7] is used for Heikin Ashi high values
    SCFloatArrayRef haLow = sc.Subgraph[8].Data; // Assuming Subgraph[8] is used for Heikin Ashi low values
    SCFloatArrayRef haClose = sc.Subgraph[9].Data; // Assuming Subgraph[9] is used for Heikin Ashi close values
    haOpen[sc.Index] = (sc.Open[sc.Index ] + sc.Close[sc.Index]) / 2;
    haClose[sc.Index] = (sc.Open[sc.Index] + sc.High[sc.Index] + sc.Low[sc.Index] + sc.Close[sc.Index]) / 4;
    haHigh[sc.Index] = max(sc.High[sc.Index], max(haOpen[sc.Index], haClose[sc.Index]));
    haLow[sc.Index] = min(sc.Low[sc.Index], min(haOpen[sc.Index], haClose[sc.Index]));
  
    float closeRSI = f_zrsi(haClose, length, sc);
    float openRSI = closeRSI;
    float highRSI_raw = f_zrsi(haHigh, length, sc);
    float lowRSI_raw = f_zrsi(haLow, length, sc);
    float highRSI = max(highRSI_raw, lowRSI_raw);
    float lowRSI = min(highRSI_raw, lowRSI_raw);
    *close = (openRSI + highRSI + lowRSI + closeRSI) / 4;

    // message.Format("haclose:%.5f",haClose);
    // sc.AddMessageToLog(message,1);
    // message.Format("closeRSI :%.5f", closeRSI);
    // sc.AddMessageToLog(message,1);
    // message.Format("openRSIl:%.5f",openRSI);
    // sc.AddMessageToLog(message,1);
    // message.Format("highRSI_RAW:%.5f",highRSI_raw);
    // sc.AddMessageToLog(message,1);
    static float prev_open = 0;
    if (sc.Index == 0)
    {
        *open = (openRSI + closeRSI) / 2;
    }
    else
    {
        *open = (prev_open * smothingget+ (*close)) / (smothingget + 1);
    }
    prev_open = *open;
    *high = max(highRSI, max((*open), (*close)));
    *low = min(lowRSI, min((*open), (*close)));
       message.Format("*close------>  %.5f",*close);
    sc.AddMessageToLog(message,1);

}

SCSFExport scsf_NetVolumeCalculation(SCStudyInterfaceRef sc)
{
    SCString message;
   
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
    SCInputRef BarSise=sc.Input[11];
    SCInputRef Bullishcolor=sc.Input[12];
    SCInputRef BearishColor=sc.Input[13];
    COLORREF backgroundcolor= RGB(128, 128, 128); //defalut background color
         // Color definitions
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

    // // Define colors for shapes
    // COLORREF color_red = RGB(255, 0, 0);          // Red for extreme sell shapes
    // COLORREF color_orange = RGB(255, 165, 0);     // Orange for extreme sell shapes
    // COLORREF color_yellow = RGB(255, 255, 0);      // Yellow for extreme sell shapes
    // COLORREF color_white = RGB(255, 255, 255);     // White for shapes
    // COLORREF color_purple = RGB(128, 0, 128);      // Purple for extreme buy shapes
    // COLORREF color_fuchsia = RGB(255, 0, 255);     // Fuchsia for extreme buy shapes
    // COLORREF color_aqua = RGB(0, 255, 255);        // Aqua for extreme buy shapes
     // Color definition
    // COLORREF frontier_buy_sweep_color =  RGB(128, 128, 128); //rgb(51, 49, 49)
    // COLORREF frontier_sell_sweep_color =  RGB(128, 128, 128); //rgb(51, 49, 49)

    // Set default input values
    if (sc.SetDefaults)
    {
       
        sc.GraphName = "Net Volume of Positive and Negative Volume VV2";
        sc.StudyDescription = "Net Volume of Positive and Negative Volume VV2 with Heikin Ashi and RSI calculations";
        sc.AutoLoop = 1;

        LengthHARSI.Name = "Length HARSI";
        LengthHARSI.SetFloat(14);
        LengthHARSI.SetFloatLimits(1, INT_MAX);

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
        BarSise.Name="Bar Size";
        BarSise.SetFloat(12);
        BarSise.SetFloatLimits(1,INT_MAX);

        ShowZones.Name = "Show Bullish/Bearish Zones";
        ShowZones.SetYesNo(true);
        sc.Subgraph[0].Name = "Bullish Zone";
        sc.Subgraph[1].Name = "Bearish Zone";
        sc.Subgraph[2].Name = "Neutral Zone";
        sc.Subgraph[0].DrawStyle=DRAWSTYLE_BACKGROUND;
        sc.Subgraph[1].DrawStyle=DRAWSTYLE_BACKGROUND;
        sc.Subgraph[2].DrawStyle=DRAWSTYLE_BACKGROUND;
        sc.Subgraph[0].PrimaryColor = RGB(33, 150, 243); // Blue for bullish
        sc.Subgraph[1].PrimaryColor = RGB(255, 82, 82); // Red for bearish
        sc.Subgraph[2].PrimaryColor = RGB(120, 123, 134); // Gray for neutral
        sc.Subgraph[4].Name = "CNV_TB";
        sc.Subgraph[4].DrawStyle = DRAWSTYLE_BAR;
        sc.Subgraph[4].PrimaryColor =  RGB(120, 123, 134);
        sc.Subgraph[6].Name="haopen"; // Assuming Subgraph[6] is used for Heikin Ashi open values
        sc.Subgraph[7].Name="haHigh"; // Assuming Subgraph[7] is used for Heikin Ashi high values
        sc.Subgraph[8].Name="halow"; // Assuming Subgraph[8] is used for Heikin Ashi low values
        sc.Subgraph[9].Name="haClose"; // Assuming Subgraph[9] is used for Heikin Ashi close values
        Bullishcolor.Name="Bullishcolor";
        Bullishcolor.SetColor(sc.Subgraph[0].PrimaryColor);

        BearishColor.Name="BearishColor";
        BearishColor.SetColor(sc.Subgraph[1].PrimaryColor);
        return;
    }

    smothingget=Smoothing.GetFloat();
    float open=0.0f;
    float high=0.0f; 
    float low=0.0f ;
    float close=0.0f;
    message.Format("LengthHARSI.GetFloat()------>: %.5f",LengthHARSI.GetFloat());
    sc.AddMessageToLog(message,1);
    f_rsiHeikinAshi(LengthHARSI.GetFloat(), sc, &open, &high, &low, &close);
        // Determine line color
   
      // Main zones
    bool extreme_sell = close > UpperOBExtreme.GetFloat();
    bool frontier_sell = close < UpperOBExtreme.GetFloat() && close > UpperOB.GetFloat();
    bool sea_zone = close < UpperOB.GetFloat() && close > LowerOS.GetFloat();
    bool frontier_buy = close < LowerOS.GetFloat() && close > LowerOSExtreme.GetFloat();
    bool extreme_buy = close < LowerOSExtreme.GetFloat();

    bool fr_buy_sweep = sea_zone && sc.High[sc.Index] > UpperOB.GetFloat();
    bool fr_sel_sweep = sea_zone && sc.Low[sc.Index] < LowerOS.GetFloat();
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

  
    float nv=0;
    float change_1 = sc.Close[sc.Index] - sc.Close[sc.Index - 1];
    nv = change_1 > 0 ? sc.Volume[sc.Index] : change_1 < 0 ? -sc.Volume[sc.Index] : 0;  
    SCSubgraphRef  cnvArray = sc.Subgraph[3]; // Temporary array for CNV values
    SCSubgraphRef  smaArray = sc.Subgraph[4]; // Temporary array for SMA values
    cnvArray[sc.Index] =cnvArray[sc.Index-1] +nv;
    // smaArray[sc.Index]=close;
    sc.SimpleMovAvg(cnvArray, smaArray, sc.Index, Length.GetFloat());
    static float cnv_tb =0;
    cnv_tb= cnvArray[sc.Index]-smaArray[sc.Index];
     message.Format("cnvarryasume :%.5f",cnvArray[sc.Index]);
    sc.AddMessageToLog(message,1);
    message.Format("nv : %.5f",nv);
    sc.AddMessageToLog(message,1);
    message.Format("cnv_tb++++++++++++++++++>: %.5f",cnv_tb);
    sc.AddMessageToLog(message,1);
    bool bullishRule = cnv_tb > 0;
    bool bearishRule = cnv_tb <= 0;
    static int ruleState = 0;
    if (bullishRule)
        ruleState = 1;
    else if (bearishRule)
        ruleState = -1;
    else
        ruleState = ruleState;
  if (ShowZones.GetYesNo())
    {   
        if (ruleState == 1)
           {
            sc.Subgraph[0].Data[sc.Index] = 1; // Mark bullish zone
            sc.Subgraph[1].Data[sc.Index] = 0; // No bearish zone
            sc.Subgraph[2].Data[sc.Index] = 0; // No neutral zone
            sc.Subgraph[0].PrimaryColor = Bullishcolor.GetColor(); // Blue for bullish

           }
        else if (ruleState == -1)
           {
            sc.Subgraph[0].Data[sc.Index] = 0; // No bullish zone
            sc.Subgraph[1].Data[sc.Index] = 1; // Mark bearish zone
            sc.Subgraph[2].Data[sc.Index] = 0; // No neutral zone
            sc.Subgraph[1].PrimaryColor = BearishColor.GetColor();
           }
        else
           {
            sc.Subgraph[0].Data[sc.Index] = 0; // No bullish zone
            sc.Subgraph[1].Data[sc.Index] = 0; // No bearish zone
            sc.Subgraph[2].Data[sc.Index] = 1; // Mark neutral zone
            sc.Subgraph[2].PrimaryColor = RGB(128, 128, 128); // Gray for neutral
           }
    }

    // Alert conditions based on crossover and crossunder of CNV threshold with zero line
    bool upp = (cnv_tb > 0 && sc.Index > 0 && cnvArray[sc.Index-1] <= 0);
    bool downn = (cnv_tb <= 0 && sc.Index > 0 && cnvArray[sc.Index-1] > 0);
    // Add alerts based on conditions
   if (upp)
       {
        message.Format("Buy Signal");
	    sc.AddMessageToLog(message, 1);
       }
   if (downn)
       {
        message.Format("Sell Signal");
	    sc.AddMessageToLog(message, 1);
       }
     COLORREF lineColor = RGB(128, 128, 128); // Default to gray
    if (cnv_tb > 0)
    {
        if (extra_extreme_sell)
            lineColor = extra_extreme_sell_color;
        else if (extreme_sell_1)
            lineColor = extreme_sell_1_color;
        else if (extreme_sell_2)
            lineColor = extreme_sell_2_color; // Orange
        else if (extreme_sell_3)
            lineColor = extreme_sell_3_color; // Yellow
        else if (extreme_sell_4)
            lineColor = extreme_sell_4_color; // White
        else if (fr_buy_sweep)
            lineColor = frontier_buy_sweep_color;
    }
    else
    {
        if (extra_extreme_buy)
            lineColor = extra_extreme_buy_color;
        else if (extreme_buy_1)
            lineColor = extreme_buy_1_color; // Fuchsia
        else if (extreme_buy_2)
            lineColor = extreme_buy_2_color; // Purple
        else if (extreme_buy_3)
            lineColor =extreme_buy_3_color; // Aqua
        else if (extreme_buy_4)
            lineColor = extreme_buy_4_color; // White
        else if (fr_sel_sweep)
            lineColor = frontier_sell_sweep_color;
    }
    // Plot CNV with determined color
    sc.Subgraph[4].DataColor[sc.Index] = lineColor;
    sc.Subgraph[4][sc.Index] = cnv_tb;
    sc.Subgraph[4].LineWidth=BarSise.GetFloat();

}




            lineColor = extreme_buy_1_color; // Fuchsia
        else if (extreme_buy_2)
            lineColor = extreme_buy_2_color; // Purple
        else if (extreme_buy_3)
            lineColor =extreme_buy_3_color; // Aqua
        else if (extreme_buy_4)
            lineColor = extreme_buy_4_color; // White
        else if (fr_sel_sweep)
            lineColor = frontier_sell_sweep_color;
    }
    // Plot CNV with determined color
    sc.Subgraph[4].DataColor[sc.Index] = lineColor;
    sc.Subgraph[4][sc.Index] = cnv_tb;
    sc.Subgraph[4].LineWidth=BarSise.GetFloat();

}



