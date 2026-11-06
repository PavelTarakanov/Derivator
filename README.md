# Symbolic Differentiator & Taylor Series Generator

A C++ application for symbolic differentiation, mathematical expression simplification, Taylor series expansion, and interactive LaTeX/Graphviz output generation.

---

## 🛠 Features

- **Symbolic Differentiation**: Computes $N$-th order derivatives of mathematical expressions.
- **Taylor Expansion**: Expands expressions into Taylor series up to a user-specified degree.
- **Expression Simplification**: Evaluates constant subexpressions and removes arithmetic identities (e.g., $0 \cdot x = 0$, $1 \cdot x = x$).
- **Visual & LaTeX Dumps**: Generates `.tex` files for mathematical formulas and visual tree representations via Graphviz (`.dot` / `.png`).

---

## 🚀 Building the Project

Requirements:
- `g++` (supporting C++11 or higher)
- `make`
- `graphviz` (optional, for visual tree rendering)


---

## 📂 Usage & Input Files

### 1. Preparing the Input File
The program requires an input text file containing a valid mathematical expression in **infix notation**, terminated by a `$` symbol.

**Expression Rules:**
- Operations: `+`, `-`, `*`, `/`, `^`, unary `-`
- Supported functions: `sin`, `cos`, `tg`, `ctg`, `arcsin`, `arccos`, `arctg`, `arcctg`, `ln`, `exp`, `sqrt`
- Variables: Multi-character variable names using letters and underscores (e.g., `x`, `var_1`).
- Numbers: Integer and floating-point constants.
- The input string **must end with a `$` marker**.

#### Example Input File (`input.txt`):
```text
(x^2 + sin(x)) / (2 * x)$
```

### 2. Running the Program
Pass the path to your input file as a **command-line argument**:

```bash
./derivator "input.txt"
```

### 3. Interactive Prompts
Upon launch, the program will prompt you in the terminal for runtime parameters depending on the active mode (e.g., `derivator` or `Teilor`):

1. **Derivative order**: Enter an integer (e.g., `1` for the first derivative).
2. **Variable of differentiation**: Enter the target variable name (e.g., `x`).
3. **Variable values**: Enter numeric values for all identified variables at the point of differentiation.

---

## 📊 Output Files

Execution produces the following output files in the working directory:

- `derivating.tex`: Contains step-by-step LaTeX equations of the differentiation process.
- `latex_dump.tex`: Contains the final simplified derivative formatted in LaTeX.
- `teilor_dump.tex`: Contains the generated Taylor series expansion (if enabled).
- `dump.txt` / `visual_dump.png`: Graphviz visualization of the expression syntax tree.
