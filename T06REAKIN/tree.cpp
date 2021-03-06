#include "tree.h"

err_t tree_ctor( tree_t* const tree, err_t* const errp, const char* const dump_name )
{
    errp_verify(errp, error);

    /* ptrs check */
    if (isBadPtr (tree) || isBadPtr (dump_name))
    {
        *errp = BAD_PTR;
        return  BAD_PTR;
    }

    /* tree fields check */
    if (tree->size_ != 0 || tree->root_ != nullptr || tree->dump_name_ != nullptr)
    {
        *errp = BAD_INIT_ERR;
        return  BAD_INIT_ERR;
    }

    /* tree setup */
    tree->dump_name_    = dump_name;
    tree->size_         = 1;
    tree->root_         = (node_t* )calloc (1, sizeof (node_t));

    /* allocation error */
    if (tree->root_ == nullptr)
    {
        *errp = MEM_ERR;
        return MEM_ERR;
    }

    tree->root_->root = tree->root_;

    /* init is good */
    return OK;
}

err_t tree_dstr( tree_t* const tree, err_t* const errp )
{
    errp_verify(errp, error);

    if (err_t error = tree_verificator (tree))
    {
        *errp = error;
        return error;
    }

    /* rec tree nodes deleting */
    tree_del_subt (tree, tree->root_, errp);

    if (!*errp)
    {
        /* resetting data */
        tree->size_         = 0;
        tree->root_         = nullptr;
        tree->dump_name_    = nullptr;
    }

    return OK;
}

node_t* tree_get_left( const tree_t* const tree, const node_t* const nodep, err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    return nodep->left_ch;
}

node_t* tree_get_righ( const tree_t* const tree, const node_t* const nodep, err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    return nodep->righ_ch;
}

node_t* tree_get_prnt( const tree_t* const tree, const node_t* const nodep, err_t* const errp )
{
    tree_full_verify (tree, nodep, nullptr, errp);

    return nodep->parent;
}

data_t  tree_get_data( const tree_t* const tree, const node_t* const nodep, err_t* const errp )
{
    tree_full_verify (tree, nodep, POISON_, errp);

    return nodep->data;
}

int     tree_get_dept( const tree_t* const tree, const node_t* const nodep, err_t* const errp )
{
    tree_full_verify (tree, nodep, BAD_DEPTH_, errp);

    return nodep->depth;
}

node_t* tree_add( tree_t* const tree, node_t* const nodep, bool side, err_t* const errp )/* ���������� �������� left ��� righ */
{
    tree_full_verify (tree, nodep, nullptr, errp);

    /* can't attach new node here */
    if (( nodep->left_ch != nullptr || side ) && ( nodep->righ_ch != nullptr || !side ))
    {
        *errp = ADD_ERR;
        return nullptr;
    }

    /* allocating memory */
    node_t* child = (node_t* )calloc (1, sizeof (node_t));
    if (child == nullptr)
    {
        *errp = MEM_ERR;
        return nullptr;
    }

    /* initing child node fields */
    child->parent = nodep;
    child->root   = nodep->root;
    child->depth  = nodep->depth + 1;

    if (!side)
        nodep->left_ch = child;
    else
        nodep->righ_ch = child;

    tree->size_++;

    /* returning new node ptr */
    return child;
}

err_t tree_set_data( tree_t* const tree, node_t* const nodep, data_t const data, err_t* const errp )
{
    tree_full_verify (tree, nodep, error, errp);

    /* writing new data to node */
    nodep->data = data;

    return OK;
}

err_t tree_del_subt( tree_t* const tree, node_t* const nodep, err_t* const errp )
{
    tree_full_verify (tree, nodep, error, errp);

    return *errp = tree_sub_t_del (tree, nodep);
}

err_t tree_sub_t_del( tree_t* const tree, node_t* const nodep )
{
    /* there should not be nullptrs */
    if (nodep == nullptr)
    {
        return BAD_PTR;
    }

    /* deleting left child */
    if (nodep->left_ch != nullptr)
    {
        if (err_t error = tree_sub_t_del (tree, nodep->left_ch))
        {
            return error;
        }
    }

    /* deleting righ child */
    if (nodep->righ_ch != nullptr)
    {
        if (err_t error = tree_sub_t_del (tree, nodep->righ_ch))
        {
            return error;
        }
    }

    /* nulling parent ptr */
    if (nodep->parent != nullptr)
    {
        if (nodep == nodep->parent->left_ch)
        {
            nodep->parent->left_ch = nullptr;
        }

        else
        {
            nodep->parent->righ_ch = nullptr;
        }
    }

    /* free memory */
    free (nodep);

    /* tree size decr */
    tree->size_--;

    return OK;
}

node_t* tree_slow_search_for_data( const tree_t* const tree, const data_t* const data_to_search, err_t* const errp )
{
    errp_verify (errp, nullptr);
    tree_verify (tree, nullptr, errp);

    /* data node search */
    node_t* node = tree_sub_t_search (tree, tree->root_, data_to_search);

    /* returning found data node */
    return node;
}

node_t* tree_sub_t_search ( const tree_t* const tree, node_t* const node, const data_t* const to_cmpr )
{
    /* comparing data */
    if (!compare_data_f (&(node->data), to_cmpr))
    {
        return node;
    }

    /* searching for data in right sub tree */
    if (node->righ_ch != nullptr)
    {
        node_t* c_node = tree_sub_t_search (tree, node->righ_ch, to_cmpr);

        if (c_node != nullptr)
        {
            return c_node;
        }
    }
    
    /* searching for data in left sub tree */
    if (node->left_ch)
    {
        node_t* c_node = tree_sub_t_search (tree, node->left_ch, to_cmpr);

        if (c_node != nullptr)
        {
            return c_node;
        }
    }

    /* not found */
    return nullptr;
}

err_t tree_is_nodep_valid( const tree_t* const tree, const node_t* const nodep )
{
    if (isBadPtr (nodep))
    {
        return BAD_PTR;
    }

    if (nodep->root != tree->root_)
    {
        return ROOT_MATCH_ERR;
    }

    return OK;
}

/* verification func */
err_t tree_verificator( const tree_t* const tree )
{
    if (tree->root_->left_ch != nullptr)
    {
        if (err_t error = tree_sub_t_ver (tree, tree->root_->left_ch))
        {
            return error;
        }
    }

    if (tree->root_->righ_ch != nullptr)
    {
        if (err_t error = tree_sub_t_ver (tree, tree->root_->righ_ch))
        {
            return error;
        }
    }

    return OK;
}

err_t tree_sub_t_ver( const tree_t* const tree, const node_t* const nodep )
{
    node_sub_t ntype = tree_get_node_st (tree, nodep);

    switch (ntype)
    {
        case ROOT:
        case BODY:
        case LEAF:
        {
            break;
        }

        case UNDEF:
        case FAKE_ROOT:
        case WRONG_LEFT:
        case WRONG_RIGHT:
        case WRONG_PARENT:
        {
            return (err_t)ntype;
        }

        default:
        {
            fprintf (stderr, "\n\n" "UNDEF NODE TYPE" "\n\n");
        }
    }

    if (nodep->left_ch != nullptr)
    {
        if (err_t error = tree_sub_t_ver (tree, nodep->left_ch))
        {
            return error;
        }
    }

    if (nodep->righ_ch != nullptr)
    {
        if (err_t error = tree_sub_t_ver (tree, nodep->righ_ch))
        {
            return error;
        }
    }

    return OK;
}

int numof_dumped_nodes_ = 0;

void tree_dump( const tree_t* const tree )
{
    if (isBadPtr (tree))
    {
        fprintf (stderr, "\n\n" "BAD PTR IN DUMP FUNC!" "\n\n");
    }

    /* max size of dump str about one data unit */
    static const int max_dump_str_len = 500;

    /* allocating memory for dump buffer */
    char *buff = (char* )calloc (sizeof (char) * max_dump_str_len, tree->size_ + 1);
    if (buff == nullptr)
    {
        fprintf (stderr, "\n\nNO MEMORY FOR DUMP BUFFER!\n\n");
    }

    /* shift in dump buffer */
    size_t buff_shift = 0;

    /* init info about graph */
    buff_shift += sprintf (buff + buff_shift, "strict digraph G{\n"
                                               "rankdir=LR; \n"
                                               "edge[color=\"black\"]; \n"
                                               "node[color=\"black\", shape=record];\n");

    /* recoursive tree dump */
    buff_shift += tree_sub_t_dmp (tree, tree->root_, buff + buff_shift);
    numof_dumped_nodes_ = 0;

    /* end of graph */
    buff_shift += sprintf (buff + buff_shift, "}");

    /* creating file */
    FILE* out_file = fopen ("dump_file.dot", "wb");
    if (out_file == nullptr)
    {
        free (buff);
        return;
    }

    /* writing tree info and free memory */
    fwrite (buff, sizeof (char), buff_shift, out_file);
    free (buff);
    fclose (out_file);

    /* system cmd str */
    char cmd[300] = "\0";

    /* dump */
    sprintf (cmd, "dot -Tpdf dump_file.dot > %s", tree->dump_name_);
    system (cmd);

    /* dump out */
    sprintf (cmd, "%s", tree->dump_name_);
    system (cmd);

    return;
}

int   tree_sub_t_dmp( const tree_t* const tree, const node_t* const nodep, char* const buff )
{
    if (isBadPtr (buff))
    {
        fprintf (stderr, "\n\n" "BAD BUFF PTR IN DUMP" "\n\n");
        return 0;
    }

    /* diff colors for left and right nodes */
    const char body_colors[2][8] = { "#cccc00", "#b38600" };

    /* left ro right */
    static bool tree_side = 0;

    /* helps if tree cycled */
    if (numof_dumped_nodes_ >= tree->size_)
    {
        return 0;
    }

    /* can't dump it */
    if (isBadPtr (nodep))
    {
        return 0;
    }

    char data_str[MAX_STR_LEN_] = "UNKNOWN DATA";

    get_data_str (nodep->data, data_str);

    size_t buff_shift = 0;

    /* node info print define */
    #define NODE_PRINT( color, node_type )                                                                                                  \
    {                                                                                                                                       \
        case node_type:                                                                                                                     \
        {                                                                                                                                   \
            buff_shift = sprintf (buff,                                                                                                     \
                "\nNODE%p[style=\"filled\", fillcolor=\"%s\", pin = \"true\", "                                                             \
                "label=\"<top> %p | <left> left  %p  | <right> right  %p |<father> father  %p| %s | depth %d | \\\"%.*s\\\"\"]; \n",        \
                 nodep, color, nodep, nodep->left_ch, nodep->righ_ch, nodep->parent, #node_type, nodep->depth, MAX_STR_LEN_, data_str);     \
            break;                                                                                                                          \
        }                                                                                                                                   \
    }

    /* node info out */
    switch (tree_get_node_st (tree, nodep))
    {
        NODE_PRINT ("#FF1D72", WRONG_RIGHT);
        NODE_PRINT ("#FF1D72", WRONG_LEFT);
        NODE_PRINT ("#FF1D72", WRONG_PARENT);
        NODE_PRINT ("#FF1D72", UNDEF);

        NODE_PRINT ("#ffcc66", ROOT);
        NODE_PRINT ("#33cc33", LEAF);

        NODE_PRINT (body_colors[tree_side], BODY);

        default:
        {
            fprintf (stderr, "\n\n" "UNDEF TYPE IN DUMP" "\n\n");
        }
    }

    #undef NODE_PRINT( color, node_type )

    /* PTRS INFO */
    if (nodep->left_ch != nullptr)
    { 
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<left>->NODE%p:<top>;\n",
                                                    nodep, nodep->left_ch);
    }

    if (nodep->righ_ch != nullptr)
    { 
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<right>->NODE%p:<top>;\n",
                                                    nodep, nodep->righ_ch);
    }

    if (!isBadPtr (nodep->left_ch))
    {
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<father>->NODE%p:<top>;\n", nodep->left_ch,  nodep->left_ch->parent);
    }

    if (!isBadPtr (nodep->righ_ch))
    {
        buff_shift += sprintf (buff + buff_shift, "NODE%p:<father>->NODE%p:<top>;\n", nodep->righ_ch, nodep->righ_ch->parent);
    }

    /* ********* */

    numof_dumped_nodes_++;

    /* recoursion */
    if (nodep->left_ch != nullptr)
    {
        tree_side = 0;
        buff_shift += tree_sub_t_dmp (tree, nodep->left_ch, buff + buff_shift);
    }

    if (nodep->righ_ch != nullptr)
    {
        tree_side = 1;
        buff_shift += tree_sub_t_dmp(tree, nodep->righ_ch, buff + buff_shift);
    }

    return buff_shift;
}

node_sub_t tree_get_node_st( const tree_t* const tree, const node_t* const nodep )
{
    if (tree_is_nodep_valid (tree, nodep))
    {
        return UNDEF;
    }

    /* checking father ptr */
    if (isBadPtr (nodep->parent))
    {
        if (nodep->parent == nullptr)
        {
            if (tree->root_ != nodep)
            {
                return FAKE_ROOT;
            }

            return ROOT;
        }

        return WRONG_PARENT;
    }

    /* checking left ptr */
    if (isBadPtr (nodep->left_ch))
    {
        if (nodep->left_ch == nullptr)
        {
            if (nodep->righ_ch == nullptr)
            {
                return LEAF;
            }

            if (nodep->righ_ch->parent != nodep)
            {
                return WRONG_RIGHT;
            }

            return BODY;
        }

        return WRONG_LEFT;
    }

    else if (nodep->left_ch->parent != nodep)
    {
        return WRONG_LEFT;
    }

    /* checking right ptr */
    if (isBadPtr (nodep->righ_ch))
    {
        if (nodep->righ_ch == nullptr)
        {
            if (nodep->left_ch == nullptr)
            { 
                return LEAF;
            }

            if (nodep->left_ch->parent != nodep)
            {
                return WRONG_LEFT;
            }

            return BODY;
        }

        return WRONG_RIGHT;
    }

    else if (nodep->righ_ch->parent != nodep)
    {
        return WRONG_RIGHT;
    }

    /* father ptr check */
    if (nodep->parent->left_ch != nodep && nodep->parent->righ_ch != nodep)
    {
        return WRONG_PARENT;
    }

    return BODY;
}

err_t tree_err_check( err_t* errp )
{
    /* error is undefined */
    if (isBadPtr (errp))
    {
        return BAD_ERR_PTR;
    }

    return *errp;
}