#include "pch.h"
#include "CppUnitTest.h"
#include "..\BloomFilterCpp\BloomFilter.h"
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace BloomFilterCppTest
{
	TEST_CLASS(BloomFilterCppTest)
	{
	public:
		std::wstring GenerateRandomString()
		{
			std::random_device randomDevice;
			std::mt19937 twisterEngine(randomDevice());

			std::wstring someCharacters(L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-=[],.!@#$%^&*()_+{}|:<>?"); // Skip semi-colon
			std::shuffle(someCharacters.begin(), someCharacters.end(), twisterEngine);

			return someCharacters.substr(0, ((rand() % someCharacters.length()) + 1));     
		}
		
		TEST_METHOD(VerifyConstructionDestruction)
		{
			BloomFilter* bloomFilter = nullptr;

			try
			{
				bloomFilter = new BloomFilter();
			}
			catch (...)
			{
				Assert::Fail(L"Failed to create filter");
			}

			try
			{
				delete bloomFilter;
				bloomFilter = nullptr;
			}
			catch (...)
			{
				Assert::Fail(L"Failed to destruct filter");
			}

			Assert::IsNull(bloomFilter);
		}

		TEST_METHOD(VerifySimpleAddCheck)
		{
			auto bloomFilter = new BloomFilter();

			const wchar_t* someElement = L"Starship";
			const wchar_t* someElement1 = L"Apurva23232";

			// Add only one of the two
			bloomFilter->Add(someElement);

			Assert::AreEqual(bloomFilter->Check(someElement), true);
			Assert::AreEqual(bloomFilter->Check(someElement1), false);
		}

		TEST_METHOD(VerifyCaseSentivity)
		{
			auto bloomFilter = new BloomFilter();

			const wchar_t* someElement = L"Starship";
			const wchar_t* someElement1 = L"starship";

			// Add mixed-case element
			bloomFilter->Add(someElement);

			Assert::AreEqual(bloomFilter->Check(someElement), true);
			Assert::AreEqual(bloomFilter->Check(someElement1), false); // lower-case element should be false
		}

		TEST_METHOD(VerifyEmptyFilterCheck)
		{
			auto bloomFilter = new BloomFilter();
			std::vector<wstring> addedElements;

			int maxWords = 1000;
			int numTrueChecks = 0;

			for (int i = 0; i < maxWords; i++)
			{
				std::wstring someElement(GenerateRandomString());
				addedElements.push_back(std::move(someElement));
			}

			std::for_each(addedElements.begin(), addedElements.end(),
				[&bloomFilter, &numTrueChecks](std::wstring const& someElement)
				{
					if (bloomFilter->Check(someElement.c_str()))
					{
						numTrueChecks++;
					}
				});

			Assert::AreEqual(numTrueChecks, 0);
		}

		TEST_METHOD(ShowFalsePositiveStats)
		{
			auto bloomFilter = new BloomFilter();
			std::vector<wstring> addedElements;

			int maxWords = 1000000;
			int numTrueChecks = 0;

			for (int i = 0; i < maxWords; i++)
			{
				std::wstring someElement(GenerateRandomString());
				bloomFilter->Add(someElement.c_str());
				addedElements.push_back(std::move(someElement));
			}

			std::for_each(addedElements.begin(), addedElements.end(),
				[&bloomFilter, &numTrueChecks](std::wstring const& someElement)
				{
					auto modElement = someElement;
					modElement.replace(0, 1, L";"); // Semi-colon was not used in the master list.
					if (bloomFilter->Check(modElement.c_str()))
					{
						numTrueChecks++;
					}
				});

			wchar_t formattedString[1024];
			swprintf(formattedString, wcslen(formattedString), L"Stats: %d out of %d were false positives.", numTrueChecks, maxWords);
			Logger::WriteMessage(formattedString);
		}

		TEST_METHOD(VerifyFalseNegatives)
		{
			auto bloomFilter = new BloomFilter();
			std::vector<wstring> addedElements;

			int maxWords = 1000000;
			int numTrueChecks = 0;

			for (int i = 0; i < maxWords; i++)
			{
				std::wstring someElement(GenerateRandomString());
				bloomFilter->Add(someElement.c_str());
				addedElements.push_back(std::move(someElement));
			}

			std::for_each(addedElements.begin(), addedElements.end(),
				[&bloomFilter, &numTrueChecks](std::wstring const& someElement)
				{
					if (bloomFilter->Check(someElement.c_str()))
					{
						numTrueChecks++;
					}
				});

			Assert::AreEqual(numTrueChecks, maxWords);
		}
	};
}
