#include "sierrachart.h"
SCDLLName("HAkesni Zone")
SCSFExport scsf_HeikenAshiRSI(SCStudyInterfaceRef sc)
{
    SCSubgraphRef HASourceOpen = sc.Subgraph[0];
    SCSubgraphRef HASourceHigh = sc.Subgraph[1];
    SCSubgraphRef HASourceLow = sc.Subgraph[2];
    SCSubgraphRef HASourceClose = sc.Subgraph[3];

    SCInputRef RSILength = sc.Input[0];
    SCInputRef Smoothing = sc.Input[1];
    SCInputRef UpperOB = sc.Input[2];
    SCInputRef UpperExtremeOB = sc.Input[3];
    SCInputRef LowerOS = sc.Input[4];
    SCInputRef LowerExtremeOS = sc.Input[5];

    if (sc.SetDefaults)
    {
        sc.GraphName = "Heiken Ashi RSI";
        sc.AutoLoop = 1;

        RSILength.Name = "RSI Length";
        RSILength.SetInt(14);
        RSILength.SetIntLimits(1, MAX_STUDY_LENGTH);

        Smoothing.Name = "Open Smoothing";
        Smoothing.SetInt(1);
        Smoothing.SetIntLimits(1, 100);

        UpperOB.Name = "Overbought";
        UpperOB.SetInt(20);
        UpperOB.SetIntLimits(1, 50);

        UpperExtremeOB.Name = "Overbought Extreme";
        UpperExtremeOB.SetInt(30);
        UpperExtremeOB.SetIntLimits(1, 50);

        LowerOS.Name = "Oversold";
        LowerOS.SetInt(-20);
        LowerOS.SetIntLimits(-50, -1);

        LowerExtremeOS.Name = "Oversold Extreme";
        LowerExtremeOS.SetInt(-30);
        LowerExtremeOS.SetIntLimits(-50, -1);

        HASourceOpen.Name = "HA Open";
        HASourceHigh.Name = "HA High";
        HASourceLow.Name = "HA Low";
        HASourceClose.Name = "HA Close";

        return;
    }

    // Calculate Heiken Ashi Values
    SCSubgraphRef HAOpenArray=sc.Subgraph[5];
    SCSubgraphRef HAHighArray=sc.Subgraph[6];
    SCSubgraphRef HALowArray=sc.Subgraph[7];
    SCSubgraphRef HACloseArray=sc.Subgraph[8];
    sc.HeikinAshi(sc.BaseDataIn, HAOpenArray, sc.Index, 14, 14);

    SCFloatArrayRef RSISource=sc.Subgraph[10].Data;
    SCSubgraphRef RSIValues=sc.Subgraph[9];

    // RSI Calculation based on Heiken Ashi Close
    sc.RSI(RSISource, RSIValues,sc.Index, RSILength.GetInt());

    // Adjust RSI to be zero-centered
    for (int i = sc.UpdateStartIndex; i < sc.ArraySize; ++i)
    {
        float closeRSI = RSIValues[i] - 50;
        float openRSI = i > 0 ? RSIValues[i - 1] - 50 : closeRSI;

        float highRSI_raw = sc.RSI(HAHighArray, RSIValues, RSILength.GetInt())[i] - 50;
        float lowRSI_raw = sc.RSI(HALowArray, RSIValues, RSILength.GetInt())[i] - 50;
        
        float highRSI = max(highRSI_raw, lowRSI_raw);
        float lowRSI = min(highRSI_raw, lowRSI_raw);

        float open = Smoothing.GetInt() == 1 ? (openRSI + closeRSI) / 2 :
            (HASourceOpen[i - 1] * Smoothing.GetInt() + closeRSI) / (Smoothing.GetInt() + 1);
        
        float high = max(highRSI, max(open, closeRSI));
        float low = min(lowRSI, min(open, closeRSI));

        // Store results in subgraphs
        HASourceOpen[i] = open;
        HASourceHigh[i] = high;
        HASourceLow[i] = low;
        HASourceClose[i] = closeRSI;  
    }
}
