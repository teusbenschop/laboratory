#!/usr/bin/env python

# https://pythonspot.com/python-graph/

import networkx as nx

graph = {
  'A': ['B', 'C'],
  'B': ['C', 'A'],
  'C': ['D'],
  'D': ['A']
}

print(graph)

G = nx.Graph()
G.add_node ("A")
G.add_node ("B")
G.add_node ("C")
G.add_edge ("A","B")
G.add_edge ("B","C")
G.add_edge ("C","A")

print ("Nodes: " + str(G.nodes()))
print ("Edges: " + str(G.edges()))

