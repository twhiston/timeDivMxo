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
    
    int numVals;
    sint16      valuelist[22];
    
} t_asctTimeDiv;

///////////////////////// function prototypes
//// standard set
void *asctTimeDiv_new(t_symbol *s, long argc, t_atom *argv);
void asctTimeDiv_free(t_asctTimeDiv *x);
void asctTimeDiv_assist(t_asctTimeDiv *x, void *b, long m, long a, char *s);
void asctTimeDiv_int(t_asctTimeDiv *x, long n);
void asctTimeDiv_float(t_asctTimeDiv *x, double f);
void asctTimeDiv_list(t_asctTimeDiv *x, t_symbol *s, long argc, t_atom *argv);
void asctTimeDiv_default(t_asctTimeDiv *x);

//////////////////////// global class pointer variable
void *asctTimeDiv_class;


int C74_EXPORT main(void)
{
	t_class *c;
	
	c = class_new("asctTimeDiv", (method)asctTimeDiv_new, (method)asctTimeDiv_free, (long)sizeof(t_asctTimeDiv),
				  0L /* leave NULL!! */, A_GIMME, 0);
	
	/* you CAN'T call this from the patcher */
    class_addmethod(c, (method)asctTimeDiv_assist,			"assist",		A_CANT, 0);
    class_addmethod(c, (method)asctTimeDiv_int,"int",A_LONG, 0);	// the method for an int in the left inlet (inlet 0)
    class_addmethod(c, (method)asctTimeDiv_float,"float",A_FLOAT, 0);// the method for an int in the left inlet (inlet 0)
    class_addmethod(c, (method)asctTimeDiv_list, "anything", A_GIMME, 0);
	
	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	asctTimeDiv_class = c;
    
	return 0;
}

void asctTimeDiv_list(t_asctTimeDiv *x, t_symbol *s, long argc, t_atom *argv)
{
    if (strncmp (s->s_name,"time",4) == 0)
    {
        
        uint8 i; //note that i is deliberately OUT of the scope of the loop
        t_atom *ap;
        sint16 timedivs[22];
        //calloc(timedivs, sizeof(sint16)*22);
        timedivs[0] = -1;//the first is always do nothing
        
        // increment ap each time to get to the next atom
        for (i = 0, ap = argv; i < argc; i++, ap++) {
            switch (atom_gettype(ap)) {
                case A_SYM:
                    if (strncmp (atom_getsym(ap)->s_name,"default",7) == 0) {
                        asctTimeDiv_default(x);
                        return; //after this we kick out of the routine as default always forces default
                    }

                    if (strncmp (atom_getsym(ap)->s_name,"1nd",3) == 0)
                        timedivs[i+1] = 2800;
                    else if (strncmp (atom_getsym(ap)->s_name,"1n",3) == 0)
                        timedivs[i+1] = 1920;
                    else if (strncmp (atom_getsym(ap)->s_name,"1nt",3) == 0)
                        timedivs[i+1] = 1280;
                    else if (strncmp (atom_getsym(ap)->s_name,"2nd",3) == 0)
                        timedivs[i+1] = 1440;
                    else if (strncmp (atom_getsym(ap)->s_name,"2n",3) == 0)
                        timedivs[i+1] = 960;
                    else if (strncmp (atom_getsym(ap)->s_name,"2nt",3) == 0)
                        timedivs[i+1] = 640;
                    else if (strncmp (atom_getsym(ap)->s_name,"4nd",3) == 0)
                        timedivs[i+1] = 720;
                    else if (strncmp (atom_getsym(ap)->s_name,"4n",3) == 0)
                        timedivs[i+1] = 480;
                    else if (strncmp (atom_getsym(ap)->s_name,"4nt",3) == 0)
                        timedivs[i+1] = 320;
                    else if (strncmp (atom_getsym(ap)->s_name,"8nd",3) == 0)
                        timedivs[i+1] = 360;
                    else if (strncmp (atom_getsym(ap)->s_name,"8n",3) == 0)
                        timedivs[i+1] = 240;
                    else if (strncmp (atom_getsym(ap)->s_name,"8nt",3) == 0)
                        timedivs[i+1] = 160;
                    else if (strncmp (atom_getsym(ap)->s_name,"16nd",4) == 0)
                        timedivs[i+1] = 180;
                    else if (strncmp (atom_getsym(ap)->s_name,"16n",4) == 0)
                        timedivs[i+1] = 120;
                    else if (strncmp (atom_getsym(ap)->s_name,"16nt",4) == 0)
                        timedivs[i+1] = 80;
                    else if (strncmp (atom_getsym(ap)->s_name,"32nd",4) == 0)
                        timedivs[i+1] = 90;
                    else if (strncmp (atom_getsym(ap)->s_name,"32n",4) == 0)
                        timedivs[i+1] = 60;
                    else if (strncmp (atom_getsym(ap)->s_name,"32nt",4) == 0)
                        timedivs[i+1] = 40;
                    else if (strncmp (atom_getsym(ap)->s_name,"64nd",4) == 0)
                        timedivs[i+1] = 45;
                    else if (strncmp (atom_getsym(ap)->s_name,"64n",4) == 0)
                        timedivs[i+1] = 30;
                    else if (strncmp (atom_getsym(ap)->s_name,"128n",4) == 0)
                        timedivs[i+1] = 15;
                    break;
                default:
                    return;
            }
        }
        //once we have the atoms shove them into the value list, blank out the rest
        for (int a = 0; a<22; a++) {
            if (a < i+1) {
                x->valuelist[a] = timedivs[a];
            } else {
                x->valuelist[a] = 0;
            }
            post("%i",x->valuelist[a]);
        }
        x->numVals = i+1;
    }
}

void asctTimeDiv_default(t_asctTimeDiv *x)
{
    static sint16 timedivs[22] = {-1,2880,1920,1280,1440,960,640,720,480,320,360,240,160,180,120,80,90,60,40,45,30,15};

    for (int i = 0; i<22; i++) {
        x->valuelist[i] = timedivs[i];
        post("%i",x->valuelist[i]);
    }
    x->numVals = 22;
    post("made default time list");
}

void asctTimeDiv_assist(t_asctTimeDiv *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "raw ticks = output, int = set division, 'time list' sets new time values. Default list has 21");
	} 
	else {	// outlet
		sprintf(s, "outlets tick count at interval point %ld", a);
	}
}

void asctTimeDiv_free(t_asctTimeDiv *x)
{
	;
}

//we dont really need to protect this value but we will
void asctTimeDiv_int(t_asctTimeDiv *x,long n)
{
    //initial value check
    if(n<0)
        n=0;
    if(n>x->numVals-1)
        n=x->numVals-1;
    //dont under-run
    if (n <= 0) {
        x->sel[0] = x->valuelist[n];
    } else {
        x->sel[0] = x->valuelist[n-1];
    }
    x->sel[1] = x->valuelist[n];
    //dont over-run
    if (n == x->numVals-1) {
        x->sel[2] = x->valuelist[x->numVals-1];
    } else {
        x->sel[2] = x->valuelist[n+1];
    }
    x->changeFlag = 1;
}

// if we do a float we are calculating a value
// as the selected value will always be +/-1
// it needs to calculate values either side of the selected so we can instantly output it
void asctTimeDiv_float(t_asctTimeDiv *x, double f)
{
    double temp = 0;
    uint8 i =0;
    uint8 suppressFlag = 0;
    
    //copy the current outs to the old array
    for(i=3;i--;)
        x->outOld[i] = x->out[i];
    
    for (int i = 0; i < 3;i++) {
        if (x->sel[i] != -1 && x->sel[i] != 0) {
            x->out[i] = f/x->sel[i];
        } else {
            x->out[i] = -1;
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
	
	// object instantiation
	if (x = (t_asctTimeDiv *)object_alloc(asctTimeDiv_class)) {
        x->m_outlet1 = intout((t_object *)x);
        asctTimeDiv_default(x);
	}
	return (x);
}
