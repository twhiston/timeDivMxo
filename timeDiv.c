/**
	@file
	Tom Whiston

	@ingroup	twhiston
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

////////////////////////// object struct
typedef struct _timeDiv
{
	t_object    ob;			// the object itself (must be first)
    
    long        countHistory; // previous output
    t_int8      changeFlag;   // did the division value change?
    long        out;          // current output
    
    void        *m_outlet1;        //division outlet
    void        *m_outlet2;        //raw tick outlet
    void        *m_outlet3;        //message outlet
    long        numVals;
    t_int16     valueList[22];
    long        selectedValue;
    
} t_timeDiv;

///////////////////////// function prototypes
//// standard set
void *timeDiv_new(t_symbol* s, long argc, t_atom* argv);
void timeDiv_free(t_timeDiv* x);
void timeDiv_assist(t_timeDiv* x, void* b, long m, long a, char *s);
void timeDiv_int(t_timeDiv* x, long n);
void timeDiv_float(t_timeDiv* x, double f);
void timeDiv_list(t_timeDiv* x, t_symbol* s, long argc, t_atom* argv);
void timeDiv_default(t_timeDiv* x);

//////////////////////// global class pointer variable
void *timeDiv_class;


int C74_EXPORT main(void)
{
	t_class *c;
	
	c = class_new("timeDiv", (method)timeDiv_new, (method)timeDiv_free, (long)sizeof(t_timeDiv),
				  0L /* leave NULL!! */, A_GIMME, 0);
	
	/* you CAN'T call this from the patcher */
    class_addmethod(c, (method)timeDiv_assist, "assist",   A_CANT,  0);
    class_addmethod(c, (method)timeDiv_int,    "int",      A_LONG,  0);
    class_addmethod(c, (method)timeDiv_float,  "float",    A_FLOAT, 0);
    class_addmethod(c, (method)timeDiv_list,   "anything", A_GIMME, 0);
	
	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	timeDiv_class = c;
    
	return 0;
}

// Iterate the current list and make it atoms for output
void timeDiv_iterListPost(t_timeDiv *x){
    // use this for outlet output
    t_atom argv[x->numVals+1];
    atom_setsym(argv, gensym("clear"));
    for(int i = 0; i < x->numVals; i++){
        t_int16 val = x->valueList[i];
        atom_setlong(argv + i + 1, val);
    }
    outlet_list(x->m_outlet3, 0L, x->numVals, argv);
}

void timeDiv_list(t_timeDiv *x, t_symbol *s, long argc, t_atom *argv)
{
    
    if (strncmp (s->s_name,"get",3) == 0)
    {
        timeDiv_iterListPost(x);
        return;
    } else if (strncmp (s->s_name,"tick",3) == 0)
    {
        x->numVals = argc+1;
        for (int i = 0; i < sizeof(x->valueList); i++) {
            if (i < x->numVals) {
                x->valueList[i] = atom_getlong(argv+i);
            } else {
                x->valueList[i] = 0;
            }
        }
    }
    // time and a list allows you to specify which options are available, and not the actual value
    // time 1n 2n 2nt 8n (etc...)
    // time default restores the default list
    else if (strncmp (s->s_name,"div",3) == 0)
    {
        t_uint8 i; //note that i is deliberately OUT of the scope of the loop
        t_atom* ap;
        t_int16 timedivs[22];
		int a = 0;
        timedivs[0] = -1;//the first is always do nothing
        
        // increment ap each time to get to the next atom
        for (i = 0, ap = argv; i < argc; i++, ap++) {
            switch (atom_gettype(ap)) {
                case A_SYM:
                    if (strncmp (atom_getsym(ap)->s_name,"default",7) == 0) {
                        timeDiv_default(x);
                        return;
                    }
                    
                    //This is horrible stuff, but c has no native hash/map ¯\_(ツ)_/¯
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
        // increment this twice, once for the -1 we insert at the start and once because we use it to specify the size of an array
        ++i;++i;
        //once we have the atoms shove them into the value list, blank out the rest
        for (a = 0; a<22; a++) {
            if (a < i) {
                x->valueList[a] = timedivs[a];
            } else {
                x->valueList[a] = 0;
            }
        }
        x->numVals = i;
    }
}

void timeDiv_default(t_timeDiv* x)
{
    static t_int16 timedivs[22] = {-1,2880,1920,1280,1440,960,640,720,480,320,360,240,160,180,120,80,90,60,40,45,30,15};
    for (int i = 0; i<22; i++) {
        x->valueList[i] = timedivs[i];
    }
    x->numVals = 23;
}

void timeDiv_assist(t_timeDiv *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "raw ticks = output, int = set division, list = see help");
	} 
	else {	// outlet
		sprintf(s, "outlets tick count at interval point %ld", a);
	}
}

//If it's an int, it is choosing a value from the list of divisions
void timeDiv_int(t_timeDiv *x, long n)
{
    // bounds check
    if(n>x->numVals-1)
        n=x->numVals-1;
    if (n < 0)
        n = 0;
    
    x->selectedValue = n;
    x->changeFlag = 1;
}

// if we do a float we are calculating a value
void timeDiv_float(t_timeDiv *x, double f)
{

    x->countHistory = x->out;
    
    t_int16 divisorValue = x->valueList[x->selectedValue];
    
    //make the new out
    if (divisorValue > 0) {
        x->out = f/divisorValue;
    } else {
        x->out = 0;
    }
    
    //if there is a changeflag we need to do a history calculation
    if(x->changeFlag == 1){
        if (divisorValue > 0) {
            x->countHistory = (f-1.0)/divisorValue;
        } else {
            x->countHistory = -1;
        }
        x->changeFlag = 0;
    }
    
    //if value changes output it
    if (x->out != x->countHistory) {
        outlet_int(x->m_outlet1, x->out);
        outlet_float(x->m_outlet2, f);
    }
}

void *timeDiv_new(t_symbol *s, long argc, t_atom *argv)
{
	t_timeDiv *x = NULL;
	
	// object instantiation
    if ((x = (t_timeDiv *)object_alloc(timeDiv_class))) {
        x->m_outlet3 = outlet_new((t_object *)x, NULL);
        x->m_outlet2 = floatout((t_object *)x);
        x->m_outlet1 = intout((t_object *)x);
        timeDiv_default(x);
        x->selectedValue = 0;
	}
	return (x);
}

void timeDiv_free(t_timeDiv *x)
{
    ;
}
