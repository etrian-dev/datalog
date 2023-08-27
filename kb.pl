likes(maria,john).
alive(john).
alive(maria).
loves(X, Y) :- likes(X, Y), alive(X), alive(Y).
