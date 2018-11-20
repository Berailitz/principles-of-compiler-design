class Node(object):
    def __init__(self, word: str = None):
        self.word = word
        self.branch_dict = dict()


root = Node()
word_list = sorted(['abc', 'acb', 'abb'])
max_length = max([len(word) for word in word_list])
for i in range(max_length):
    next_char = word_list[i]
    if next_char not in next_node_dict.keys():
        next_node_dict[next_char] = Node(next_char)
