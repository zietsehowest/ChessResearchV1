# Introduction
My goal for this project was to implement an AI that doesn't necessarily need to understand chess but tries to play chess against a human opponent. First I started to search
for an algorithm that predicts or even thinks. But it turns out that implementing like for example a neural network to actually teach it to play chess is way harder than it looks. 
So I settled with the minimax algorithm which is a algorithm that uses a binary tree depth search method.

# explaining the algorithm
Minimax is mostly used in zero-sum game theory. This means that the algorithm tries to minimze the opponen't maximum payoff. In zero-sum games this is Identical to minimzing one's own loss, and to maximizeing one's own minimum gain.

So take this Tree for example:

![Image](Images/Image1.png)

**Step1:** It is the minimum player its turn. So he goes through the tree and takes from every base branch the minimum value to the node parent node.

**Step2:** Now it is the player that represents the maximum's turn. As previous he checks al child nodes and now takes the maximums of its children.

So this is the basics of the minmax algorithm.

# Applying the algorithm on a chess board

To apply chess to the minimax algorithm we first need to determine some things. To begin with every piece needs a certain value. Because we are going to calculate our tree with the determined values for each piece. This is how we are going to know what the difference is between taking a pawn and taking a rook for example.

![MinimaxChess](Images/Image2.jpeg)

* **MAX PART**
As you can see in this Image white needs to move now (white is assigned as the player that always tries to maximize its score). White can choose between 2 options or take the knight or take the bishop. Taking the bishop results to a score of -80 because the layers below are also calculated. And taking the bishop results in a score of -50 because its white's turn he chooses to take the bishop because that results in a higher score down the line.

* **MIN PART**
Now as the second move it is blacks turn. Black tries to get a score as low as possible so it choses the move that results in the lowest score possible.

---
Now there is 1 thing we haven't discussed. When executing this algorithm we choose a certain depth. This depth represents the amount of Parents and Children that we traverse to get the highest or lowest score possible. But increasing this depth is at a high cost because imagine taking depth 1 we search 1 node. But taking depth 2 it becomes 2 possibilities and so on. On this image you can clearly see that this is very performance heavy.

![PerformanceMinimax](Images/Image3.png)

So how can we fix this ? Introducing Alpha Beta pruning !

# Alpha Beta Pruning with MiniMax Algorithm

What Alpha Beta Pruning does is while traversing nodes to check for possible scores it checks when entering a branch of a certain node if the parent node is already lower or higher then the given node. When this is true we already know that the player won't even need the underlaying branches to achieve its highest possible score. We can see how this works clearly on these images

![MinimaxWithoutAB](Images/Image4.png) ![MinimaxWithAB](Images/Image5.png)
