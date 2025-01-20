#include "sierrachart.h"

/*============================================================================
	Example study function for interacting with spreadsheets.
----------------------------------------------------------------------------*/
SCSFExport scsf_SpreadsheetInteractionExample(SCStudyInterfaceRef sc)
{
	SCSubgraphRef Subgraph_FormulaResult = sc.Subgraph[0];

	if (sc.SetDefaults)
	{
		// Set the configuration and defaults

		sc.GraphName = "Spreadsheet Interaction Example";

		sc.StudyDescription
			= "This is an example study for demonstrating how to interact with spreadsheets from ACSIL.  This example requires an open sheet collection name \"ACSILInteractionExample\".";

		sc.AutoLoop = 0;

		Subgraph_FormulaResult.Name = "Formula Result";
		Subgraph_FormulaResult.DrawZeros = true;

		return;
	}

	// This can either contain a complete path and file extension, or just
	// the name of the spreadsheet file itself without the extension.
	const char* SheetCollectionName = "ACSILInteractionExample";

	const char* SheetName = "Sheet1";

	void* SheetHandle = sc.GetSpreadsheetSheetHandleByName(SheetCollectionName, SheetName, false);

	// Note: The Column and Row parameters are zero-based indexes, so column
	// 'A' is index 0, and row '1' is index 0.

	// Set labels in column A.
	sc.SetSheetCellAsString(SheetHandle, 0, 1, "High");
	sc.SetSheetCellAsString(SheetHandle, 0, 2, "Low");
	sc.SetSheetCellAsString(SheetHandle, 0, 3, "Enter Formula");
	sc.SetSheetCellAsString(SheetHandle, 0, 5, "Log Message");

	// Set values in column B.
	sc.SetSheetCellAsDouble(SheetHandle, 1, 1, sc.BaseData[SC_HIGH][sc.Index]);
	sc.SetSheetCellAsDouble(SheetHandle, 1, 2, sc.BaseData[SC_LOW][sc.Index]);

	// Get the result from cell B4. Column and row indexes are zero-based.
	double CellValue = 0.0;
	sc.GetSheetCellAsDouble(SheetHandle, 1, 3, CellValue);

	// Set the result into a subgraph.
	Subgraph_FormulaResult[sc.Index] = static_cast<float>(CellValue);

	// Get the text from cell B6, if it exists, and add it to the message log.
	SCString CellString;
	if (sc.GetSheetCellAsString(SheetHandle, 1, 5, CellString))
		sc.AddMessageToLog(CellString, 0);
}

/*==========================================================================*/
