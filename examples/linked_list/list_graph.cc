#include <iostream>
#include "cobjectgraph.h"
#include "list.h"

using namespace std;
using namespace CObjectGraph;

namespace CObjectGraph {

    COG_DEFINE_NODE(ListNode);

    COG_WRITE_NODE_LABEL(ListNode)
    {
        oss << "str: '" << object->str << "'\\l";
        oss << "x: " << object->x << "\\l";
    }

    COG_ADD_RELATED_OBJECTS(ListNode)
    {
        auto next_node = graph->AddNode(object->next);
        graph->AddEdge(this, next_node, "next");
        graph->SetSameRank(this, next_node);
    }

    COG_SET_NODE_ATTRIBUTES(ListNode)
    {
        if (object == nullptr)
            SetAttribute("shape", "none");
    }


    COG_DEFINE_NODE(ListNode *);

    COG_WRITE_NODE_LABEL(ListNode *)
    {
        oss << var_name;
    }

    COG_ADD_RELATED_OBJECTS(ListNode *)
    {
        auto obj_node = graph->AddNode(*object);
        graph->AddEdge(this, obj_node, "");
    }

    COG_SET_NODE_ATTRIBUTES(ListNode *)
    {
        SetAttribute("shape", "oval");
    }
}

int main()
{
    LinkedList list;
    list.AddToHead("Phoenix", 110);
    list.AddToHead("Flagstaff", 85);
    list.AddToTail("Los Angeles", 90);

    Graph g("", true);

    g.SetAttribute(AttributeScope::ALL_NODES, "shape", "box");
    g.SetAttribute(AttributeScope::ALL_NODES, "fontname", "Courier New");
    g.SetAttribute(AttributeScope::ALL_NODES, "fontsize", "14");
    g.SetAttribute(AttributeScope::ALL_EDGES, "fontname", "Courier New");
    g.SetAttribute(AttributeScope::ALL_EDGES, "fontsize", "12");

    g.AddNode(&list.head, "head");
    g.AddNode(&list.tail, "tail");
    g.PrintDot();
    return 0;
}
