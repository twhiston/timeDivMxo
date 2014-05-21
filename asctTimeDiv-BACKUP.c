/**
	@file
	asctTimeDiv - Do time divisions for the polyarp in a cpu sensitive way
	Tom Whiston

	@ingroup	examples	
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

#define NUMVALS 15

////////////////////////// object struct
typedef struct _asctTimeDiv
{
	t_object    ob;			// the object itself (must be first)
    long        sel[3];
    uint8       changeFlag;
    long        out[3];
    long        outOld[3];
    void        *m_outlet1;        //the outlet
} t_asctTimeDiv;

///////////////////////// function prototypes
//// standard set
void *asctTimeDiv_new(t_symbol *s, long argc, t_atom *argv);
void asctTimeDiv_free(t_asctTimeDiv *x);
void asctTimeDiv_assist(t_asctTimeDiv *x, void *b, long m, long a, char *s);
void asctTimeDiv_int(t_asctTimeDiv *x, long n);
void asctTimeDiv_float(t_asctTimeDiv *x, double f);

//////////////////////// global class pointer variable
void *asctTimeDiv_class;


int C74_EXPORT main(void)
{	
	// object initialization, OLD STYLE
	// setup((t_messlist **)&simplemax_class, (method)simplemax_new, (method)simplemax_free, (short)sizeof(t_simplemax), 
	//		0L, A_GIMME, 0);
    // addmess((method)simplemax_assist,			"assist",		A_CANT, 0);  
	
	// object initialization, NEW STYLE
	t_class *c;
	
	c = class_new("asctTimeDiv", (method)asctTimeDiv_new, (method)asctTimeDiv_free, (long)sizeof(t_asctTimeDiv),
				  0L /* leave NULL!! */, A_GIMME, 0);
	
	/* you CAN'T call this from the patcher */
    class_addmethod(c, (method)asctTimeDiv_assist,			"assist",		A_CANT, 0);
    class_addmethod(c, (method)asctTimeDiv_int,"int",A_LONG, 0);	// the method for an int in the left inlet (inlet 0)
    class_addmethod(c, (method)asctTimeDiv_float,"float",A_FLOAT, 0);// the method for an int in the left inlet (inlet 0)
	
	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	asctTimeDiv_class = c;

	post("I am the asctTimeDiv object");
	return 0;
}

void asctTimeDiv_assist(t_asctTimeDiv *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "I am inlet %ld", a);
	} 
	else {	// outlet
		sprintf(s, "I am outlet %ld", a); 			
	}
}

void asctTimeDiv_free(t_asctTimeDiv *x)
{
	;
}

//we dont really need to protect this value but we will
void asctTimeDiv_int(t_asctTimeDiv *x,long n)
{
    int i =0;
    //an int in the inlet sets our division value
    if(n<0){
        n = 0;
    }
    if(n>=NUMVALS){
        n = NUMVALS-1;
    }
    
    //we dont protect the range of these values as we use out of bounds checking when we calculate
    x->sel[0] = n-1;
    x->sel[1] = n;
    x->sel[2] = n+1;
    x->changeFlag = 1;
    
    post("x->sel[0] %d",x->sel[0]);
    post("x->sel[1] %d",x->sel[1]);
    post("x->sel[2] %d",x->sel[2]);
}

// if we do a float we are calculating a value
// as the selected value will always be +/-1
// it needs to calculate values either side of the selected so we can instantly output it
void asctTimeDiv_float(t_asctTimeDiv *x, double f)
{
    
    post("got a float and it is %.2f", f);
    double temp = 0;
    uint8 i =0;
    uint8 suppressFlag = 0;
    
    //copy the current outs to the old array
    for(i=3;i--;)
        x->outOld[i] = x->out[i];
    
    for (int i=0;i<3;i++) {
        temp = 0;
        switch (x->sel[i]) {
            case 0:
                x->out[i] = -1;
                break;
            case 1:
                x->out[i] = (f/4)/480;
                break;
            case 2:
                x->out[i] = (f/3)/480;
                break;
            case 3:
                x->out[i] = (f/2)/480;
                break;
            case 4:
                x->out[i] = (f/1.5)/480;
                break;
            case 5:
                temp = f*3;
                x->out[i] = (temp/4)/480;
                break;
            case 6:
                x->out[i] = f/480;
                break;
            case 7:
                temp = f*3;
                x->out[i] = (temp/2)/480;
                break;
            case 8:
                x->out[i] = (f/0.5)/480;
                break;
            case 9:
                x->out[i] = (f*3)/480;
                break;
            case 10:
                x->out[i] = (f*4)/480;
                break;
            case 11:
                x->out[i] = (f*6)/480;
                break;
            case 12:
                x->out[i] = (f*8)/480;
                break;
            case 13:
                x->out[i] = (f*12)/480;
                break;
            case 14:
                x->out[i] = (f*16)/480;
                break;
            default:
                x->out[i] = -1;
                break;
        }
    }

    //if there is a changeflag we need to make sure that our new value was not in any of the old arrays
    //before we decide to output or not
    if(x->changeFlag == 1){
        for (i = 3; i--;) {
            if(x->out[1] == x->outOld[i]){
                suppressFlag = 1;
            }
        }
        x->changeFlag = 0;
    }
    
    if(suppressFlag == 0){
        if(x->out[1] != x->outOld[1] && x->out[1] != -1){
            outlet_int(x->m_outlet1, x->out[1]);
        }
    }
    
}

void *asctTimeDiv_new(t_symbol *s, long argc, t_atom *argv)
{
	t_asctTimeDiv *x = NULL;
    
	// object instantiation, OLD STYLE
	// if (x = (t_simplemax *)newobject(simplemax_class)) {
	// 	;
	// }
	
	// object instantiation, NEW STYLE
	if (x = (t_asctTimeDiv *)object_alloc(asctTimeDiv_class)) {
        object_post((t_object *)x, "a new %s object was instantiated: %p", s->s_name, x);
        object_post((t_object *)x, "it has %ld arguments", argc);
        x->m_outlet1 = intout((t_object *)x);
	}
	return (x);
}
