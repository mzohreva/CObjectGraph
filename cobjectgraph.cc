#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include "cobjectgraph.h"

using namespace std;
using namespace CObjectGraph;

int BaseNode::counter = 1;

BaseNode::BaseNode()
{
    stringstream ss;
    ss << "node" << BaseNode::counter++;
    this->name = ss.str();
}

string BaseNode::GetName() const
{
    return this->name;
}


Edge::Edge(BaseNode * from, BaseNode * to, string label)
{
    this->from = from;
    this->to = to;
    this->label = label;
}

string Edge::ToDot()
{
    stringstream ss;
    ss << this->from->GetName();
    ss << " -> " << this->to->GetName();
    ss << " [label=\"" << this->label << "\"]";
    return ss.str();
}


BaseNode * Graph::FindNodeForObject(const void * object)
{
    for (int i = 0; i < nodes.size(); i++)
        if (nodes[i]->RepresentsObject(object))
            return nodes[i].get();
    return nullptr;
}

void Graph::AddEdge(const void * fromObject, const void * toObject, string label)
{
    BaseNode * from = FindNodeForObject(fromObject);
    BaseNode * to   = FindNodeForObject(toObject);
    if (from == nullptr)
    {
        throw runtime_error("Could not find the node for fromObject");
    }
    if (to == nullptr)
    {
        throw runtime_error("Could not find the node for toObject");
    }
    Edge * e = new Edge(from , to, label);
    edges.push_back(unique_ptr<Edge>(e));
}

void Graph::SetSameRankByNode(BaseNode * obj1Node, BaseNode * obj2Node)
{
    vector< BaseNode * > s;
    if (obj1Node == nullptr)
    {
        throw runtime_error("obj1Node cannot be null");
    }
    if (obj2Node == nullptr)
    {
        throw runtime_error("obj2Node cannot be null");
    }
    s.push_back(obj1Node);
    s.push_back(obj2Node);
    rankings.push_back(s);
}

void Graph::SetSameRank(const void* obj1, const void* obj2)
{
    BaseNode * obj1Node = FindNodeForObject(obj1);
    BaseNode * obj2Node = FindNodeForObject(obj2);
    if (obj1Node == nullptr)
    {
        throw runtime_error("Could not find the node for obj1");
    }
    if (obj2Node == nullptr)
    {
        throw runtime_error("Could not find the node for obj2");
    }
    SetSameRankByNode(obj1Node, obj2Node);
}

void Graph::SetAttribute(AttributeScope scope, std::string key, std::string value)
{
    if (scope == AttributeScope::SPECIFIC_NODE)
        throw logic_error("Cannot set a node-specific attribute on Graph!");
    set_attribute(attributes, key, value, scope);
}

void Graph::PrintDot()
{
    cout << "digraph " << title << " {\n";
    // Print attributes
    for (const auto& a : attributes)
    {
        switch (a.scope)
        {
            case AttributeScope::GRAPH:
                cout << "    " << a.key << " = " << "\"" << a.value << "\";\n";
                break;

            case AttributeScope::ALL_NODES:
                cout << "    node [ " << a.key << " = " << "\"" << a.value << "\" ]\n";
                break;

            case AttributeScope::ALL_EDGES:
                cout << "    edge [ " << a.key << " = " << "\"" << a.value << "\" ]\n";
                break;

            case AttributeScope::SPECIFIC_NODE:
                throw runtime_error("Node-specific attibute in Graph!");
        }
    }
    cout << "\n";
    // Print nodes
    for (const auto& n : nodes)
    {
        cout << "    " << n->ToDot() << "\n";
    }
    cout << "\n";
    // Print rankings
    for (const auto& r : rankings)
    {
        cout << "    { rank=same; ";
        for (const auto& n : r)
        {
            cout << n->GetName() << " ";
        }
        cout << " }\n";
    }
    cout << "\n";
    // Print edges
    for (const auto& e : edges)
    {
        cout << "    " << e->ToDot() << "\n";
    }
    cout << "}\n";
}

