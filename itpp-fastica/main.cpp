#include<sndfile.hh>
#include <itpp/itsignal.h>
#include <cstdio>

using namespace itpp;
using namespace std;

vec wavread(const char* filename);
void wavwrite(const char* filename, vec& vec);


int main()
{
	FILE * fpin = NULL;
	float tmp = 0.0;

	// Separate nrIC independent components in nrSamples samples
	int nrSamples = 50000, nrIC = 2;
	mat X = zeros(nrIC, nrSamples);
	X.set_row(0, wavread("mix1.wav"));
	X.set_row(1, wavread("mix2.wav"));

	cout << "=====================================" << endl;
	cout << "   Test program for FastICA / IT++   " << endl;
	cout << "=====================================" << endl;

	// Instantiate an ICA object with default parameters : SYMM approach and
	// POW3 non-linearity
	// Be sure that :
	// - nrSamples = number of samples = nb of columns of the input matrix
	// - nrIC = number of sensors = nb of rows of the input matrix
	cout << "\n==========================================================" << endl;
	cout << "Use SYMM approach and POW3 non-linearity :" << endl;
	Fast_ICA my_fastica(X);

	// Set number of independent components to separate :
	// By default, this value is taken from the dimension of
	// the input data. This line is for illustration purposes.
	// May help in some cases.
	my_fastica.set_nrof_independent_components(nrIC);
	// Perform ICA
	bool result = my_fastica.separate();

	if (result)
	{
		// Get results
		cout << "Mixing matrix = " << my_fastica.get_mixing_matrix() << endl;
		cout << "Separation matrix = " << my_fastica.get_separating_matrix() << endl;
		//cout << "Separated independent components = "
		//	<< my_fastica.get_independent_components() << endl;
		wavwrite("result1.wav", my_fastica.get_independent_components().get_row(0));
		wavwrite("result2.wav", my_fastica.get_independent_components().get_row(1));

	}
	else
	{
		cout << "Algorithm failed" << endl;
	}

	// Another test with other parameters
	cout << "\n==========================================================" << endl;
	cout << "Use Gaussian non-linearity and deflation approach :" << endl;

	Fast_ICA my_fastica2(X);

	// Set GAUSS non-linearity
	my_fastica2.set_non_linearity(FICA_NONLIN_GAUSS);

	// Use deflation approach : IC are computed one by one
	my_fastica2.set_approach(FICA_APPROACH_DEFL);

	// Perform ICA
	result = my_fastica2.separate();

	if (result)
	{
		// Get results
		cout << "Mixing matrix = " << my_fastica.get_mixing_matrix() << endl;
		cout << "Separation matrix = " << my_fastica.get_separating_matrix() << endl;
		//cout << "Separated independent components = "
		//	<< my_fastica.get_independent_components() << endl;
		wavwrite("result3.wav", my_fastica.get_independent_components().get_row(0));
		wavwrite("result4.wav", my_fastica.get_independent_components().get_row(1));
	}
	else
	{
		cout << "Algorithm failed" << endl;
	}

	// Another test which should fail
	cout << "\n==========================================================" << endl;
	cout << "Use Gaussian non-linearity and deflation approach :" << endl;

	const int rows = 10;
	const int comp = 3;
	RNG_reset(1);
	mat signal = randu(rows, 100);
	mat guess = zeros(rows, comp);

	Fast_ICA my_fastica3(signal);

	// Use deflation approach : IC are computed one by one
	my_fastica3.set_approach(FICA_APPROACH_DEFL);
	my_fastica3.set_nrof_independent_components(comp);
	my_fastica3.set_init_guess(guess);
	my_fastica3.set_max_num_iterations(100);

	// Perform ICA
	result = my_fastica3.separate();

	if (result)
	{
		// Get results
		cout << "Mixing matrix = " << my_fastica.get_mixing_matrix() << endl;
		cout << "Separation matrix = " << my_fastica.get_separating_matrix() << endl;
		//cout << "Separated independent components = "
		//	<< my_fastica.get_independent_components() << endl;
		wavwrite("result5.wav", my_fastica.get_independent_components().get_row(0));
		wavwrite("result6.wav", my_fastica.get_independent_components().get_row(1));
	}
	else
	{
		cout << "Algorithm failed" << endl;
	}

	cout << "\nEnd of Fast_ICA execution. " << endl;

	return 0;
}

vec wavread(const char* filename){
	SndfileHandle SFH(filename, SFM_READ);
	cout << "channels:" << SFH.channels() << endl;
	cout << "samplerate:" << SFH.samplerate() << endl;
	cout << "frames:" << SFH.frames() << endl;
	cout << "format:" << SFH.format() << endl;
	vec ret = zeros(SFH.frames());
	SFH.read(ret._data(), ret.size());
	return ret;
}

void wavwrite(const char* filename, vec& vec){
	SndfileHandle SFH(filename, SFM_WRITE,65541,1,8000);
	double min = 1, max = -1;
	for (int i = 0; i < vec.size(); ++i)
	{
		if (vec(i) < min)
			min = vec(i);
		if (vec(i) > max)
			max = vec(i);
	}
	if (max > abs(min))
		vec /= max;
	else
		vec /= (-min);
	if (SFH.write(vec._data(), vec.size()) != vec.size())
		cerr << "error occur when write wav file!" << endl;
}
	