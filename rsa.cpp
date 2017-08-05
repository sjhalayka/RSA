#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <ctime>
using namespace std;


// https://mattmccutchen.net/bigint/
#include "BigInteger.hh"
#include "BigIntegerUtils.hh"
#include "BigIntegerAlgorithms.hh"


void vector_of_bytes_to_biginteger(const vector<unsigned char> &v, BigInteger &ret)
{
	ret = dataToBigInteger<unsigned char>(&v[0], v.size(), BigInteger::positive);
}

// to do make return value a pass by reference to save on construction time when returning
void biginteger_to_vector_of_bytes(vector<unsigned char> &ret, const BigInteger &b)
{
	ret.resize(0);

	ostringstream oss;
	oss << hex << b;
	string hex_string = oss.str();
	
	// put in leading "0" if hex_string's length is an odd number
	if(hex_string.length() % 2 != 0)
		hex_string = "0" + hex_string;

	for(size_t i = 0; i < hex_string.length(); i += 2)
	{
		string combined;
		combined += hex_string[i];
		combined += hex_string[i + 1];

		istringstream iss(combined);
		short signed int ssi = 0;
		iss >> hex >> ssi;
		ret.insert(ret.begin(), static_cast<unsigned char>(ssi));
	}
}

BigUnsigned GCD(BigUnsigned a, BigUnsigned b)
{
    while(b != 0)
    {
        BigUnsigned tmp = b;
        b = a % b;
        a = tmp;
    }
 
    return a;
}

BigUnsigned LCM(const BigUnsigned a, const BigUnsigned b)
{
    BigUnsigned ret;
    
    ret = a * b;
    return ret / GCD(a, b);
}


bool IsPrime(BigUnsigned number)
{
    if(number<2)
        return false;
    if(number==2)
        return true;
    for(BigUnsigned i=2;i<=number/2;i++)
    {
        if(number%i==0)
            return false;
    }

	return true;
}


int main() 
{
    srand(time(0));
	//srand(123456);


	// A vector of bytes
	vector<unsigned char> data;

	// Add 64 pseudorandom bytes (64*8 == 512 bits) to salt the message data
	const size_t num_salt_bytes = 64;

	for(size_t i = 0; i < num_salt_bytes; i++)
		data.push_back(static_cast<unsigned char>(rand()%256));

	// Add message data
	data.push_back('H');
	data.push_back('e');
	data.push_back('l');
	data.push_back('l');
	data.push_back('o');
	data.push_back(' ');
	data.push_back('w');
	data.push_back('o');
	data.push_back('r');
	data.push_back('l');
	data.push_back('d');
	data.push_back('!');

	// Print the salted message
	cout << "Salted message = ";
	
	for(size_t i = 0; i < data.size(); i++)
		cout << data[i];

	cout << endl;


	// Convert message data to integer
	BigInteger plaintext;
	vector_of_bytes_to_biginteger(data, plaintext);

//	cout << "Plaintext = " << plaintext << endl;


	// 200 digit prime numbers
	// http://primes.utm.edu/lists/small/small2.html
	const BigUnsigned p = stringToBigUnsigned("54661163828798316406139641599131347203445399912295442826728168170210404446004717881354193865401223990331513412680314853190460368937597393179445867548835085746203514200061810259071519181681661892618329");
	const BigUnsigned q = stringToBigUnsigned("58021664585639791181184025950440248398226136069516938232493687505822471836536824298822733710342250697739996825938232641940670857624514103125986134050997697160127301547995788468137887651823707102007839");
	const BigUnsigned n = p*q;
    BigUnsigned totient = LCM(p - 1, q - 1);

	// greatest_common_divisor(e, totient) = 1
	const BigUnsigned e = stringToBigUnsigned("17"); // 5 digits
    const BigUnsigned d = modinv(e, totient);


	cout << "p = " << p << endl;
	cout << "q = " << q << endl;
	cout << "n = " << n << endl;
	cout << "totient = " << totient << endl;
	cout << "e = " << e << endl;
	cout << "d = " << d << endl;


	if(plaintext >= n)
	{
		cout << "Plaintext should be less than n" << endl;
		return -1;
	}
	
	if(e >= totient)
	{
		cout << "e should be less than totient (p - 1)*(q -1)" << endl;
		return -1;
	}


	// Encrypt using public key e, N
	cout << "Encrypting..." << endl;
	const BigInteger ciphertext = modexp(plaintext, e, n);

//	cout << "Ciphertext = " << ciphertext << endl;


	// Decrypt using private key d, N
	cout << "Decrypting..." << endl;
	const BigInteger plaintext2 = modexp(ciphertext, d, n);


	// Convert integer to data
	vector<unsigned char> data2;
	biginteger_to_vector_of_bytes(data2, plaintext2);

	// Remove the salt from the message data
	data2.erase(data2.begin(), data2.begin() + num_salt_bytes);

	cout << "Unsalted message = ";

	// Print the message
	for(size_t i = 0; i < data2.size(); i++)
		cout << data2[i];

	cout << endl;
	
	return 0;
}
