# Introduction
My goal for this project was to implement an AI that doesn't necessarily need to understand chess but tries to play chess against a human opponent. First I started to search
for an algorithm that predicts or even thinks. But it turns out that implementing like for example a neural network to actually teach it to play chess is way harder than it looks. 
So I settled with the minimax algorithm which is a algorithm that uses a binary tree depth search method.

# explaining the algorithm
Minimax is mostly used in zero-sum game theory. This means that the algorithm tries to minimze the opponen't maximum payoff. In zero-sum games this is Identical to minimzing one's own loss, and to maximizeing one's own minimum gain.

So take this tree for example:
