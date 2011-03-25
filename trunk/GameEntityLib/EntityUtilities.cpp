#include "./stdafx.h"
#include "EntityUtilities.h"

double ApproachAngle(double actual,double ideal, double ammount)
{
    double result;
	double dTemp=ideal-actual;

	if(dTemp<-180){dTemp+=180;}
	if(dTemp>180){dTemp-=180;}

    if(fabs(dTemp)<ammount)
    {result=ideal;}
    else
    {result=actual+dTemp;}

    if(result<0){result=result+360;}
    if(result>360){result=result+(-360);}
    return result;
}

void ApproachAngles(CVector *pvCurrent,CVector &vIdeal, double ammount)
{
    pvCurrent->c[YAW]=ApproachAngle(pvCurrent->c[YAW],vIdeal.c[YAW],ammount);
    pvCurrent->c[PITCH]=ApproachAngle(pvCurrent->c[PITCH],vIdeal.c[PITCH],ammount);
}