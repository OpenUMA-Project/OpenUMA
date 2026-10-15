# Decode-step
We start off by taking the current token's string and convert it to its token embedding using the token_embedding table, then we load in the saved KV cache. We calculate the query vector, the key vector and the value vector and then add the key and value vectors to our kv cache.

We apply positional embeddings to the k and q vectors to give the attention heads context about how far/close the tokens are to each other.

## multihead attention loop
this part can be parallelized because each attention head doesn't need any values from the other heads.

for each head, we loop through all tokens from t=0 to t=pos (current token we're at) and calculate a score which is the current token's q vector multiplied by the key vectors of past tokens (the key vectors are saved in the key-cache).
Then softmax, then we create a running weighted sum of the array we have where the weight is the value vecror (from value-cache).

We concatenate each head's weighted sum and multiply them by a learned matrix to reduce the vector.

Then we pass it through the feed forward network, and then return the final logits (probability of each token in our vocab)

# Open questions?

None

