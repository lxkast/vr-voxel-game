import sympy as sp
sp.init_printing(num_columns=0)
N = 3
A = sp.MatrixSymbol('A', N, N)
M = sp.Matrix(N, N, lambda i, j: sp.Symbol(f'a[{i}][{j}]'))
M_inv = M.inv()
det = M.det()
print(f"double det = {sp.simplify(det).evalf()};")
for i in range(N):
    for j in range(N):
        expr = sp.simplify(M_inv[i, j] * det)
        print(f"res[{i}][{j}] = ({expr.evalf()}) / det;")