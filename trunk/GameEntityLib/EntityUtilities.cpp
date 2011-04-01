#include "./stdafx.h"
#include "EntityUtilities.h"

double ApproachAngle(double actual,double ideal, double ammount)
{
    double result;
	double dTemp=ideal-actual;

	if(dTemp<-180){dTemp=360+dTemp;}
	if(dTemp>180){dTemp=actual-ideal;}
	
    if(fabs(dTemp)<fabs(ammount))
    {result=ideal;}
    else
	{result=actual+fabs(ammount)*(dTemp>0?1.0:-1.0);}

    if(result<0){result+=360;}
    if(result>360){result-=360;}
    
    //RTTRACE("ApproachAngle : act:%f ,ideal %f, dtemp %f, res %f",actual,ideal,dTemp,result);
    return result;
}

void ApproachAngles(CVector *pvCurrent,CVector &vIdeal, double ammount)
{
    pvCurrent->c[YAW]=ApproachAngle(pvCurrent->c[YAW],vIdeal.c[YAW],ammount);
    pvCurrent->c[PITCH]=ApproachAngle(pvCurrent->c[PITCH],vIdeal.c[PITCH],ammount);
}
