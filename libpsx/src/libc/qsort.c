//  quickSort
//
//  This public-domain C implementation by Darel Rex Finley.
//
// Modified by Giuseppe Gatta 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) 
{
  #define  QSORT_MAX_LEVELS  300
	
	
	int  beg[QSORT_MAX_LEVELS], end[QSORT_MAX_LEVELS], i=0, L, R, swap ;
	char piv[size];
	
	beg[0]=0; end[0]=nmemb;
	
	while (i>=0) 
	{
		L=beg[i]; R=end[i]-1;
		
		if (L<R) 
		{
			memcpy(piv, base + (size * L), size);
						
			while (L<R) 
			{
				while(compar(base + (R*size), piv) > 0 && L<R) R--;
				
				if (L<R) 
				{
					memcpy(base + (size *L), base + (size * R), size);
					L++;
				}
				
				while(compar(base + (L*size), piv) <= 0 && L<R) L++;
				
				if (L<R)
				{
					memcpy(base + (size *R), base + (size * L), size);
					R--;
				}
			}
			
			memcpy(base + (size*L), piv, size);
			
			beg[i+1]=L+1; 
			end[i+1]=end[i];
			end[i++]=L;
			
			if (end[i]-beg[i]>end[i-1]-beg[i-1])
			{
				swap=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swap;
				swap=end[i]; end[i]=end[i-1]; end[i-1]=swap; 
			}
		}
		else 
		{
			i--; 
		}
	}
}
