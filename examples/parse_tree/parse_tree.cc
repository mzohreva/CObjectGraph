#include "cobjectgraph.h"

extern "C" {
    #include "parser.h"
}

using namespace CObjectGraph;

namespace CObjectGraph {

    COG_DEFINE_NODE(char);

    COG_WRITE_NODE_LABEL(char)
    {
        char c = *object;
        if (c > ' ')
            oss << c;
        else
            oss << "ASCII " << (int) c;
    }

    COG_DEFINE_NODE(int);

    COG_WRITE_NODE_LABEL(int)
    {
        int x = *object;
        oss << x;
    }


    COG_DEFINE_NODE(struct ExprNode);

    COG_WRITE_NODE_LABEL(struct ExprNode)
    {
        oss << "expr";
    }

    COG_ADD_RELATED_OBJECTS(struct ExprNode)
    {
        graph->AddNode(object->term);
        graph->AddEdge(object, object->term, "");

        if (object->op != '\0')
        {
            const char* opp = &(object->op);
            graph->AddNode(opp);
            graph->AddNode(object->expr);

            graph->AddEdge(object, opp, "");
            graph->AddEdge(object, object->expr, "");
        }
    }


    COG_DEFINE_NODE(struct TermNode);

    COG_WRITE_NODE_LABEL(struct TermNode)
    {
        oss << "term";
    }

    COG_ADD_RELATED_OBJECTS(struct TermNode)
    {
        graph->AddNode(object->factor);
        graph->AddEdge(object, object->factor, "");

        if (object->op != '\0')
        {
            const char* opp = &(object->op);
            graph->AddNode(opp);
            graph->AddNode(object->term);

            graph->AddEdge(object, opp, "");
            graph->AddEdge(object, object->term, "");
        }
    }


    COG_DEFINE_NODE(struct FactorNode);

    COG_WRITE_NODE_LABEL(struct FactorNode)
    {
        oss << "factor";
    }

    COG_ADD_RELATED_OBJECTS(struct FactorNode)
    {
        if (object->tag == 0)
        {
            const int * nump = &(object->num);
            graph->AddNode(nump);
            graph->AddEdge(object, nump, "");
        }
        else
        {
            const char * lparenp = &(object->lparen);
            const char * rparenp = &(object->rparen);
            graph->AddNode(lparenp);
            graph->AddNode(object->expr);
            graph->AddNode(rparenp);

            graph->AddEdge(object, lparenp, "");
            graph->AddEdge(object, object->expr, "");
            graph->AddEdge(object, rparenp, "");
        }
    }
}

int main()
{
    Graph g;
    struct ExprNode * parse_tree_root;

    parse_tree_root = parse();

    g.AddNode(parse_tree_root);
    g.PrintDot();
    return 0;
}
