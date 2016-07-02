#ifndef __COBJECTGRAPH_H__
#define __COBJECTGRAPH_H__

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>

namespace CObjectGraph
{
    class Graph;

    class Position
    {
        public:
            Position() : x{0}, y{0}, set{false} { }
            Position(int x_, int y_) : x{x_}, y{y_}, set{true} { }

            void Set(int x, int y) { this->x = x; this->y = y; this->set = true; }
            bool IsSet() const { return set; }
            int X() const
            {
                if (!set) throw std::logic_error("You should set the position first!");
                return x;
            }
            int Y() const
            {
                if (!set) throw std::logic_error("You should set the position first!");
                return y;
            }
            std::string ToDot() const
            {
                if (!set) throw std::logic_error("You should set the position first!");
                std::ostringstream oss;
                oss << x << "," << y;
                return oss.str();
            }

        private:
            int x, y;
            bool set;   // Has x, y been set?
    };

    class BaseNode
    {
        public:
            BaseNode();
            std::string GetName() const;
            virtual std::string ToDot() = 0;
            virtual void SetAttribute(std::string key, std::string value) = 0;
            virtual void SetPosition(int x, int y) = 0;
            virtual bool RepresentsObject(const void * object) = 0;
            virtual ~BaseNode() { }

        private:
            std::string name;
            static int counter;
    };

    enum class AttributeScope
    {
        GRAPH,
        ALL_NODES,
        ALL_EDGES,
        SPECIFIC_NODE
    };

    struct Attribute
    {
        std::string key;
        std::string value;
        AttributeScope scope;
    };

    template <typename C>   // C must be a container of Attribute e.g. vector<Attribute>
    void set_attribute(C& c, std::string key, std::string value, AttributeScope scope)
    {
        auto f = std::find_if(std::begin(c), std::end(c),
                              [&key, &scope] (const Attribute& x) {
                                  return (x.key == key) && (x.scope == scope);
                              });
        if (f != std::end(c))
        {
            (*f).value = value;
        }
        else
        {
            c.push_back(Attribute {key, value, scope});
        }
    }

    template <typename T>
    class Node: public BaseNode
    {
        public:
            explicit Node(const T* object, std::string var_name = "")
            {
                this->object = object;
                this->var_name = var_name;

                SetNodeAttributes();
            }

            void SetPosition(int x, int y) override
            {
                pos.Set(x, y);
            }

            std::string ToDot() override
            {
                std::ostringstream oss;
                oss << this->GetName() << " [label=\"";
                (object == nullptr) ? WriteNullNodeLabel(oss) : WriteNodeLabel(oss);
                oss << "\"";

                if (pos.IsSet()) oss << ", pos=\"" << pos.ToDot() << "\"";

                for (const auto& a : attributes)
                    oss << ", " << a.key << "=\"" << a.value << "\"";
                oss << "]";
                return oss.str();
            }

            void SetAttribute(std::string key, std::string value) override
            {
                if (key == "label" || key == "pos")
                    throw std::logic_error("label and pos attributes cannot be set this way!");
                set_attribute(attributes, key, value, AttributeScope::SPECIFIC_NODE);
            }

            bool RepresentsObject(const void * object) override
            {
                return ((const void *)this->object == object);
            }

            void AddRelatedObjects(Graph * graph)
            {
                // Default implementation does nothing
            }

        private:
            const T* object;
            Position pos;
            std::string var_name;
            std::vector<Attribute> attributes;
            static const char* type_name;

            void SetNodeAttributes()
            {
                // Default implementation does nothing
            }

            void WriteNodeLabel(std::ostringstream& oss)
            {
                oss << type_name;
            }

            void WriteNullNodeLabel(std::ostringstream& oss)
            {
                oss << "null";
            }
    };

    class Edge
    {
        public:
            Edge(BaseNode * from, BaseNode * to, std::string label);
            std::string ToDot();

        private:
            BaseNode * from;
            BaseNode * to;
            std::string label;
    };

    class Graph
    {
        public:
            explicit Graph(std::string title_ = "G") : title{title_} { }

            template <typename T>
            BaseNode * AddNode(const T* object, std::string var_name = "")
            {
                return AddNodeIfNotFound(object, false, 0, 0, var_name);
            }

            template <typename T>
            BaseNode * AddNode(const T* object, int x, int y, std::string var_name = "")
            {
                return AddNodeIfNotFound(object, true, x, y, var_name);
            }

            void AddEdge(const void * from, const void * to, std::string label);
            void SetSameRankByNode(BaseNode * obj1Node, BaseNode * obj2Node);
            void SetSameRank(const void* obj1, const void* obj2);
            void SetAttribute(AttributeScope scope, std::string key, std::string value);
            void PrintDot();

        private:
            std::string title;
            std::vector< std::unique_ptr< BaseNode > > nodes;
            std::vector< std::unique_ptr< Edge > > edges;
            std::vector< Attribute > attributes;
            std::vector< std::vector< BaseNode * > > rankings;

            template <typename T>
            BaseNode * AddNodeIfNotFound(const T* object, bool set_pos, int x, int y, std::string var_name)
            {
                BaseNode * node = FindNodeForObject(object);
                if (node == nullptr)
                {
                    Node<T> * new_node = new Node<T>(object, var_name);
                    nodes.push_back(std::unique_ptr<BaseNode>(new_node));
                    node = new_node;

                    if (set_pos)
                        new_node->SetPosition(x, y);

                    if (object != nullptr)
                        new_node->AddRelatedObjects(this);
                }
                return node;
            }

            BaseNode * FindNodeForObject(const void * object);
    };
}


#define COG_DEFINE_NODE(T) \
template <> const char* CObjectGraph::Node<T>::type_name = #T;

#define COG_SET_NODE_ATTRIBUTES(T) \
template <> void CObjectGraph::Node<T>::SetNodeAttributes()

#define COG_WRITE_NODE_LABEL(T) \
template <> void CObjectGraph::Node<T>::WriteNodeLabel(std::ostringstream& oss)

#define COG_WRITE_NULL_NODE_LABEL(T) \
template <> void CObjectGraph::Node<T>::WriteNullNodeLabel(std::ostringstream& oss)

#define COG_ADD_RELATED_OBJECTS(T) \
template <> void CObjectGraph::Node<T>::AddRelatedObjects(CObjectGraph::Graph * graph)



#endif
