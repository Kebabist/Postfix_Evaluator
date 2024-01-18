#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "evaluator.h"
#include "Queue.h"

#define LOG(x) std::cout << x << std::endl

bool test(const std::string& input, double expected_result) {
    MathEvaluator parser;
    Queue<double> RPN_Values;

    // Convert the input to RPN and evaluate it
    double evaluation = parser.eval(input);

    // Check if the evaluation result is within an acceptable range
    if (evaluation >= expected_result - 0.01 && evaluation <= expected_result + 0.01) {
        LOG(input << " -> " << expected_result << " | Success");
        return false;
    }
    else {
        LOG(input << " -> " << expected_result << " (" << evaluation << ") | Failed");
        return true;
    }

}
int main() {
    while (1) {
        std::cout << "Enter an expression: ";
        std::string user_input;
        std::getline(std::cin, user_input);
        user_input.erase(std::remove_if(user_input.begin(), user_input.end(), ::isspace), user_input.end());
        try {
        MathEvaluator parser;
        double expected_result = parser.eval(user_input);

        // Call the test function
        if (!test(user_input, expected_result)) {
            LOG("Test Passed!");
        }
        else {
            LOG("Test Failed!");
        }
     }
        catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }

    }

    return 0;
}

