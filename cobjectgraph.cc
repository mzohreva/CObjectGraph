#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include "cobjectgraph.h"

using namespace std;
using namespace CObjectGraph;

int BaseNode::counter = 1;

BaseNode::BaseNode()
{
    ostringstream oss;
    oss << "node" << BaseNode::counter++;
    this->name = oss.str();
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
    ostringstream oss;
    oss << this->from->GetName();
    oss << " -> " << this->to->GetName();
    oss << " [label=\"" << this->label << "\"]";
    return oss.str();
}


BaseNode * Graph::FindNodeForObject(const void * object)
{
    for (const auto& node : nodes)
        if (node->RepresentsObject(object))
            return node.get();
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

void Graph::PrintDot(std::ostream& os)
{
    os << "digraph " << title << " {\n";
    // Print attributes
    for (const auto& a : attributes)
    {
        switch (a.scope)
        {
            case AttributeScope::GRAPH:
                os << "    " << a.key << " = " << "\"" << a.value << "\";\n";
                break;

            case AttributeScope::ALL_NODES:
                os << "    node [ " << a.key << " = " << "\"" << a.value << "\" ]\n";
                break;

            case AttributeScope::ALL_EDGES:
                os << "    edge [ " << a.key << " = " << "\"" << a.value << "\" ]\n";
                break;

            case AttributeScope::SPECIFIC_NODE:
                throw runtime_error("Node-specific attibute in Graph!");
        }
    }
    os << "\n";
    // Print nodes
    for (const auto& n : nodes)
    {
        os << "    " << n->ToDot() << "\n";
    }
    os << "\n";
    // Print rankings
    for (const auto& r : rankings)
    {
        os << "    { rank=same; ";
        for (const auto& n : r)
        {
            os << n->GetName() << " ";
        }
        os << " }\n";
    }
    os << "\n";
    // Print edges
    for (const auto& e : edges)
    {
        os << "    " << e->ToDot() << "\n";
    }
    os << "}\n";
}

