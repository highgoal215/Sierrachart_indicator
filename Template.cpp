// The top of every source code file must include this line
#include "sierrachart.h"

// For reference, refer to this page:
// https://www.sierrachart.com/index.php?page=doc/AdvancedCustomStudyInterfaceAndLanguage.php

// This line is required. Change the text within the quote
// marks to what you want to name your group of custom studies. 
SCDLLName("Custom Study DLL")

//This is the basic framework of a study function. Change the name 'TemplateFunction' to what you require.
SCSFExport scsf_TemplateFunction(SCStudyInterfaceRef sc)
{
	// Section 1 - Set the configuration variables and defaults
	if (sc.SetDefaults)
	{
		sc.GraphName = "Template Function";

		sc.AutoLoop = 1;  //Automatic looping is enabled. 
		
		sc.Subgraph[0].Name = "Name";
		sc.Subgraph[0].DrawStyle = DRAWSTYLE_LINE;
		sc.Subgraph[0].PrimaryColor = RGB (0, 255, 0);
		
		sc.Input[0].Name = "Float Input";
		sc.Input[0].SetFloat(0.0f);
		
		return;
	}
	
	
	// Section 2 - Do data processing here
	
	
}
