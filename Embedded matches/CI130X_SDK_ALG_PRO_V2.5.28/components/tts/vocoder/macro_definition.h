//Macro_Definition
#define FFT_SIZE 512
#define HALF_FFT_SIZE 256
#define SPECTRUM_SIZE 257
#define SAMPLING_RATE (16000) //Unit:Hz
#define DIV_SAMPLING_RATE (0.00006f)
#define FRAME_PERIOD_F 0.005f
#define LOWEST_F0 32.25f

#define EPS (1e-6f)

#define INPUT_DIMENSION 62 //The input dimension
#define ALPHA  0.58f //
#define GAMMA  0.0f //

//Macro_Switch
//save_intermediate_result
#define SAVE_INTERMEDIATE_RESULT 0 //Whether to save intermediate result of the master switch,1 on behalf of yes,0 on behalf of no;
#define SAVE_MGC 1 //Whether to save intermediate result of MGC,1 on behalf of yes,0 on behalf of no;
#define SAVE_LF0 1 //Whether to save intermediate result of LF0,1 on behalf of yes,0 on behalf of no;
#define SAVE_AP 1 //Whether to save intermediate result of BAP,1 on behalf of yes,0 on behalf of no;
#define SAVE_SP 1 //Whether to save intermediate result of SP,1 on behalf of yes,0 on behalf of no;
#define SAVE_F0 0 //Whether to save intermediate result of F0,1 on behalf of yes,0 on behalf of no;
#define SAVE_WAV 1 //Whether to save intermediate result of WAV,1 on behalf of yes,0 on behalf of no;


//Function_switch
#define TIME_SWITCH 1 //Whether to output operation time, Unit:ms
#define LOAD_MLPG 0 //if use MLPG,you will set INPUT_DIMENSION and load var file;
#define BATCH_PROCESS 0 //

#define KDEFAULTF0 500.0f
#define KPI 3.14159f
#define DKPI 6.28318f
#define DKPI_fs 0.00039f

#define NOISE_LENGTH 8192
#define FRAME_PERIOD 80
#define WIN_LEN 159
#define NUM_AP 3
#define NOISE_NUM 1024
