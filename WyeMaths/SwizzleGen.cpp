#include <WyeGen.hpp>

#include <vector>
#include <string>

using Wyevern::WyeGen;
using std::size_t;

std::vector<std::vector<size_t>> Permutations(const unsigned componentsSize, const unsigned maxDimensions)
{
	std::vector<std::vector<size_t>> results;

	for(unsigned dimensions = 1; dimensions <= maxDimensions; ++dimensions)
	{
		// Calculate permutations: equivalent of pow(componentsSize, dimensions)
		size_t permutations = componentsSize;
		for(unsigned i = 1; i < dimensions; ++i)
		{
			permutations *= componentsSize;
		}

		for(size_t permutation = 0; permutation < permutations; ++permutation)
		{
			std::vector<size_t> result;

			for(unsigned index = 0; index < dimensions; ++index)
			{
				// Calculate permutation divisor by pow(componentsSize, index)
				size_t divisor = (index == 0) ? 1 : componentsSize;
				for(unsigned i = 1; i < index; ++i)
				{
					divisor *= componentsSize;
				}

				const size_t componentIndex = (permutation / divisor) % componentsSize;
				result.push_back(componentIndex);
			}
			results.push_back(result);
		}
	}

	return results;
}

int main()
{
	for(int vectorIndex = 2; vectorIndex <= 4; ++vectorIndex)
	{
		const std::string vectorIndexString = std::to_string(vectorIndex);
		const std::string filename = "Vector" + vectorIndexString + ".gen.hpp";

		WyeGen vector(filename);
		vector.Comment_Multiline
		(
			"== Wyevern Game Engine ==\n"
			"This file is automatically generated on build and is internal to the engine.\n"
			"Please do not directly include or modify this file."
		);
		vector.Space();

		const std::string guard = "WYEMATHS_GENERATED_VECTOR" + vectorIndexString + "_INCLUDED";
		vector.Macro_IfDefined_Not(guard);
		vector.Macro_Define(guard);
		vector.Space();

		const std::string baseDefinition_prefix = "Swizzle<type, " + std::to_string(vectorIndex) + ", { ";

		std::string results;
		for
		(
			std::vector<std::vector<size_t>> permutations = Permutations(vectorIndex, 4);
			const std::vector<size_t>& perm : permutations
		)
		{
			constexpr auto xyzw = "xyzw";
			constexpr auto rgba = "rgba";

			if(perm.size() == 1)
			{
				for(const size_t& i : perm)
				{
					std::string definition = "\nSwizzle1D<type, " + vectorIndexString + ", ";
					definition += std::to_string(i);
					definition += "> ";
					definition.push_back(xyzw[i]);
					definition.push_back(';');

					results.append(definition);
				}
			}
			else
			{
				std::string swizzleIndices;

				std::string xyzwName;

				for(const size_t& i : perm)
				{
					xyzwName.push_back(xyzw[i]);

					swizzleIndices += std::to_string(i) + ", ";
				}

				std::string definition = "\nSwizzle<type, " + vectorIndexString + ", " + std::to_string(perm.size()) + ", std::array<std::size_t, " + std::to_string(perm.size()) + "> { " + swizzleIndices + "}> " + xyzwName + ";";
				results.append(definition);
			}
		}
		vector.Macro_Define("WYEMATHS_GENERATED_VECTOR" + vectorIndexString + "_SWIZZLES", results);

		vector.Space();
		vector.Macro_IfDefined_End();
	}
}
