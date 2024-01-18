#include "evaluator.h"

#include <cmath>
#include <map>
#include <string>

#include "Queue.h"
#include "stack.h"
#include <vector>

#define PI 3.14159265358979323846
#define E 2.71828182845904523536

typedef long double ldouble;

ldouble _factorial(ldouble x, ldouble y, bool aoe) {
	if (aoe && y == (ldouble)(int)y) {
		ldouble factorial = 1L;
		for (int i = 1; i <= y; i++)
			factorial *= i;

		return factorial;
	}
	return sqrt(2 * PI * y) * pow(y / E, y);
}

MathEvaluator::MathEvaluator() {
	populateArrays();
	aoe = true;

	externalVariables.append("pi");
	externalVariables.append("e");
}

MathEvaluator::MathEvaluator(bool accuracyOverEfficieny) {
	populateArrays();
	aoe = accuracyOverEfficieny;
}

void MathEvaluator::appendVariable(const std::string name, double& value) {
	externalVariablesMap[name] = &value;

	if (!externalVariables.inList(name)) externalVariables.append(name);
}

void MathEvaluator::deleteVariable(const std::string name) {
	externalVariablesMap.erase(name);

	externalVariables.remove(externalVariables.getIndex(name));
}

ldouble MathEvaluator::eval(const std::string expr) {
	Queue<Token> tokens = seperate(expr).infixValues;
	bool lastTokenWasOperator = false;
	int exprIndex = 0;
	for (size_t index = 0; index < tokens.size(); index++) {
		const Token& token = tokens.getData(index);
		if (token.type == OPERATOR) {
			if (lastTokenWasOperator) {
				std::string errorPart = expr.substr(exprIndex - 1, 5); // Adjust the second parameter as needed
				throw std::runtime_error("Error in expression at: " + errorPart);
			}
			lastTokenWasOperator = true;
		}
		else {
			lastTokenWasOperator = false;
		}
		exprIndex += token.value.size();

		if (token.type == OPERAND && !isdigit(token.value[0]) && externalVariables.inList(token.value) == false) {
			std::string errorPart = expr.substr(exprIndex - token.value.size(), token.value.size() + 2);
			throw std::runtime_error("Invalid operand in expression at: " + errorPart);
		}

		if (token.type == OPERATOR && operators.inList(token.value) == false) {
			std::string errorPart = expr.substr(exprIndex - token.value.size(), token.value.size() + 2);
			throw std::runtime_error("Invalid operator in expression at: " + errorPart);
		}

		if (token.type == FUNCTION && functions.inList(token.value) == false) {
			std::string errorPart = expr.substr(exprIndex - token.value.size(), token.value.size() + 2);
			throw std::runtime_error("Invalid function in expression at: " + errorPart);
		}

		if (token.type == SYMBOL && symbols.inList(token.value) == false) {
			std::string errorPart = expr.substr(exprIndex - token.value.size(), token.value.size() + 2);
			throw std::runtime_error("Invalid symbol in expression at: " + errorPart);
		}
	}


	rpn = compile(expr);


	Stack<ldouble> resultStack;

 for (size_t index = 0; index < rpn.RPNValues.size(); index++) {
	 const Token token = rpn.RPNValues.getData(index);

	 bool isOperator = token.type == OPERATOR;
	 bool isFunction = token.type == FUNCTION;
	 bool isVariable = externalVariables.inList(token.value);
	 bool isOperand = !isOperator && !isFunction && !isVariable;
	 if (isOperator) {
		 std::string functionName = operatorTranslationTable[token.value];
		 int operands = functionParameters[functionName];

		 if (operands > (int)resultStack.size()) return 0.0;

		 std::vector<long double> values(operands);

		 for (int i = 0; i < operands; i++) {
			 values[i] = resultStack.pop();
		 }

		 if (token.value == "(-)") {  // Unary minus operator
			 resultStack.push(operatorMap[token.value](values[0], 0, aoe));
		 }
		 else {
			 resultStack.push(operatorMap[token.value](values[1], values[0], aoe));
		 }
	 }


		else if (isOperand) {
			resultStack.push(std::stod(token.value));
		}

		else if (isVariable) {
			resultStack.push(*externalVariablesMap[token.value]);
		}

		if (isFunction) {
			if (functionParameters[token.value] == 1) {
				ldouble value = resultStack.pop();
				resultStack.push(functionsMap[token.value](value));
			}

			else if (functionParameters[token.value] == 2) {
				ldouble values[2];

				for (int i = 0; i < 2; i++) {
					values[i] = resultStack.pop();
				}

				resultStack.push(multipleParameterFunction[token.value](values[1], values[0]));
			}
		}

	}

 if (resultStack.size() != 1) {
	 throw std::runtime_error("Error: Invalid expression.");
 }

	ldouble result = resultStack.peek();

	rpn.RPNValues.freeAll();
	rpn.infixValues.freeAll();
	resultStack.freeAll();

	return result;
}

me_SepValues MathEvaluator::seperate(std::string infix) {

	Queue<std::string> store;
	Queue<std::string> values;
	std::string joiner;

	for (int i = 0; i < (int)infix.length(); i++) {
		std::string item(1, infix[i]);

		// Deduce Type
		bool isOperator = operators.inList(item);
		bool isSymbol = symbols.inList(item);

		if (!isOperator && !isSymbol) {
			store.append(item);
		}

		if (isOperator || isSymbol) {
			bool lastNumOperator = false;

			if (values.size() > 0) {
				if (operators.inList(values.getData(values.size() - 1))) lastNumOperator = true;
			}

			if ((int)store.size() > 0 && lastNumOperator) lastNumOperator = false;

			if (item == "-" && ((i == 0) || (lastNumOperator))) {
				// Append unary minus operator to values queue
				values.append("(-)");
			}
			else {
				if (store.size() > 0) {
					joiner = "";
					for (size_t index = 0; index < store.size(); index++) {
						const auto j = store.getData(index);
						joiner += j;
					}
					store.freeAll();
					values.append(joiner);
				}
				values.append(item == "-" ? "(-)" : item);
			}
		}
	}

	// Dump Store
	if (store.size() > 0) {
		joiner = "";
		for (size_t index = 0; index < store.size(); index++) {
			const auto j = store.getData(index);
			joiner += j;
		}
		values.append(joiner);
	}

	me_SepValues result;
	result.infix = infix;

	bool isParsingNumber = false;
	bool hasDecimalPoint = false;

	for (int i = 0; i < (int)infix.length(); i++) {
		std::string item(1, infix[i]);

		// Deduce Type
		bool isOperator = operators.inList(item);
		bool isSymbol = symbols.inList(item);
		bool isOperand = isdigit(infix[i]) || isalpha(infix[i]) || infix[i] == '.'; // assuming operands can be digits, letters, or decimal points

		if (!isOperator && !isSymbol && !isOperand) {
			throw std::runtime_error("Invalid character in expression: " + item);
		}

		if (isOperand) {
			if (infix[i] == '.') {
				if (!isParsingNumber) {
					// Start parsing a new number
					isParsingNumber = true;
					hasDecimalPoint = true;
				}
				else if (hasDecimalPoint) {
					// Already encountered a decimal point in the current number
					throw std::runtime_error("Invalid floating point number in expression: " + infix.substr(i - 1, 3));
				}
				else {
					// First decimal point in the current number
					hasDecimalPoint = true;
				}
			}
			else if (!isdigit(infix[i])) {
				// Start parsing a new number
				isParsingNumber = false;
				hasDecimalPoint = false;
			}
		}
		else {
			// Start parsing a new number
			isParsingNumber = false;
			hasDecimalPoint = false;
		}
	}

	// Deduce Type From Tokens

	Queue <Token> typedValues;

	for (size_t index = 0; index < values.size(); index++) {
		const auto i = values.getData(index);
		token_type type;

		bool isOperator = operators.inList(i);
		bool isFunction = !isOperator && functions.inList(i);
		bool isSymbol = !isOperator && !isFunction && symbols.inList(i);
		bool isOperand = !isOperator && !isFunction && !isSymbol;

		if (isOperator) type = OPERATOR;
		if (isFunction) type = FUNCTION;
		if (isSymbol) type = SYMBOL;
		if (isOperand) type = OPERAND;

		Token item = { i, type };

		typedValues.append(item);
	}

	result.infixValues = typedValues;

	store.freeAll();
	values.freeAll();

	return result;
}



me_RPN MathEvaluator::compile(const std::string infix) {

	me_SepValues sep = seperate(infix);

	Stack<Token> stack;
	Queue<Token> queue;

	for (size_t index = 0; index < sep.infixValues.size(); index++) {
		const auto i = sep.infixValues.getData(index);
		bool isOperator = i.type == OPERATOR;
		bool isFunction = i.type == FUNCTION;
		bool isSymbol = i.type == SYMBOL;

		if (!isOperator && !isFunction && !isSymbol) {
			queue.append(i);
		}

		else if (!isOperator && isFunction && !isSymbol) {
			stack.push(i);
		}

		else if (isOperator && !isFunction && !isSymbol) {
			while (stack.size() > 0) {
				if (((stack.peek().type == OPERATOR) && (operatorPrecedence[stack.peek().value] > operatorPrecedence[i.value]))
					|| ((operatorPrecedence[stack.peek().value] == operatorPrecedence[i.value]) && (operatorAssociative[stack.peek().value] == 0) && (stack.peek().value != "("))) {

					queue.append(stack.pop());
				}

				else {
					break;
				}
			}

			stack.push(i);
		}

		else if (i.value == "(" || i.value == "[") {
			stack.push(i);
		}

		else if (i.value == ")" || i.value == "]") {
			std::string matchingOpening;
			if (i.value == ")") {
				matchingOpening = "(";
			}
			else if (i.value == "]") {
				matchingOpening = "[";
			}

			while (!stack.empty() && stack.peek().value != matchingOpening) {
				queue.append(stack.pop());
			}

			// If the stack is empty, there's a mismatched closing parenthesis or bracket
			if (stack.empty()) {
				std::string errorPart = infix.substr(std::max(0, static_cast<int>(i.value.size()) - 2), std::min(static_cast<int>(i.value.size()) + 2, static_cast<int>(infix.size())));
				throw std::runtime_error("Mismatched closing parenthesis or bracket in expression at: " + errorPart);
			}

			// Pop the opening parenthesis or bracket from the stack
			stack.pop();

			// If the top of the stack is a function (like sin or cos), pop it into the queue
			if (!stack.empty() && functions.inList(stack.peek().value)) {
				queue.append(stack.pop());
			}
		}
	}

	while (stack.size() > 0) {
		if (stack.peek().value == "(" || stack.peek().value == "[") {
			std::string errorPart = infix.substr(std::max(0, static_cast<int>(stack.peek().value.size()) - 2), std::min(static_cast<int>(stack.peek().value.size()) + 2, static_cast<int>(infix.size())));
			throw std::runtime_error("Mismatched opening parenthesis or bracket in expression at: " + errorPart);
		}
		queue.append(stack.pop());
	}

	std::string joiner = "";
	Queue<Token> fixedQueue;

	for (size_t index = 0; index < queue.size(); index++) {
		const auto i = queue.getData(index);
		joiner += i.value;
		if (i.value != "") fixedQueue.append(i);
	}

	me_RPN result;

	result.infix = sep.infix;
	result.infixValues = sep.infixValues;
	result.RPNValues = fixedQueue;
	result.RPN = joiner;

	stack.freeAll();
	queue.freeAll();

	return result;
}


void MathEvaluator::populateArrays() {
	std::string symArr[5] = { "(", ")", "[", "]", "," };
	for (int i = 0; i < 5; i++) symbols.append(symArr[i]);

	for (auto const& element : operatorMap) operators.append(element.first);
	for (auto const& element : functionsMap) functions.append(element.first);
}


ldouble evaluate(const std::string infix) {
	MathEvaluator evaluator;
	return evaluator.eval(infix);
}