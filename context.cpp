#include "context.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

bool Function::Declared(char *name) {
    for(int i = 0; i < num_locals; i++) {
        Variable *var = local_variables[i];
        if(strcmp(var->node->vardecl.name, name) == 0) {
            return true;
        }
    }
    
    return false;
}

int Function::GetIndexOfLocal(char *name) {
    for(int i = 0; i < num_locals; i++) {
        Variable *var = local_variables[i];
        if(strcmp(var->node->vardecl.name, name) == 0) {
            return i+1;
        }
    }
    
    // TODO: Figure out what a invalid return value should be
}

int Context::GetIndexOfGlobal(char *name) {
    for(int i = 0; i < num_globals; i++) {
        Variable *var = global_variables[i];
        if(strcmp(var->node->vardecl.name, name) == 0) {
            return i;
        }
    }
    
    // TODO: Figure out what a invalid return value should be
}

bool Context::GlobalDeclared(char *name) {
    for(int i = 0; i < num_globals; i++) {
        Variable *var = global_variables[i];
        if(strcmp(var->node->vardecl.name, name) == 0) {
            return true;
        }
    }
    
    return false;
}

bool Context::FunctionExists(char *name) {
    for(int i = 0; i < num_functions; i++) {
        Function *function = functions[i];
        if(strcmp(function->node->funcdef.name, name) == 0) {
            return true;
        }
    }
    
    return false;
}

Function* Context::GetFunction(char *name) {
    for(int i = 0; i < num_functions; i++) {
        Function *function = functions[i];
        if(strcmp(function->node->funcdef.name, name) == 0) {
            return function;
        }
    }
    
    return NULL;
}

Node* FindFunctions(Context *context, Node *nodes) {
    Node **functions = NULL;
    int nfuncs = 0;
    
    Node *first_not_removed = NULL;
    
    Node *n = nodes;
    while(n != NULL) {
        printf("node#type: %d\n", n->type);
        
        if(n->type == NODE_FUNCDEF) {
            if(n->prev != NULL) n->prev->next = n->next;
            if(n->next != NULL) n->next->prev = n->prev;
            
            nfuncs++;
            functions = (Node**) realloc(functions, sizeof(Node*)*nfuncs);
            functions[nfuncs-1] = n;
        }else if(first_not_removed == NULL) {
            first_not_removed = n;
        }
        
        n = n->next;
    }
    
    for(int i = 0; i < nfuncs; i++) {
     
        Function *function = new Function();
        function->node = functions[i];
        
        context->num_functions++;
        context->functions = (Function**) realloc(context->functions, sizeof(Function*)*context->num_functions);
        context->functions[context->num_functions-1] = function; 
        
        function->local_variables = NULL;
        function->num_locals = 0;
        
        Node *stmts_first_not_removed = NULL;
        n = function->node->funcdef.stmts;
        while(n != NULL) {
         
            if(n->type == NODE_VARDECL) {
                
                if(n->prev != NULL) n->prev->next = n->next;
                if(n->next != NULL) n->next->prev = n->prev;
                
                Variable *var = new Variable();
                var->node = n;
                
                function->num_locals++;
                function->local_variables = (Variable**) realloc(function->local_variables, sizeof(Variable*)*function->num_locals);
                function->local_variables[function->num_locals-1] = var;
            }else if(n->type == NODE_VARDECLASSIGN) {
                // Replace this with an assign
                // adn remove the decl part
                
                n->type = NODE_ASSIGNMENT;
                char *name = n->vardeclassign.name;
                Node *expr = n->vardeclassign.expr;
                n->assignment.name = name;
                n->assignment.expr = expr;
                
                Variable *var = new Variable();
                var->node = new Node();
                var->node->type = NODE_VARDECL;
                var->node->vardecl.name = name;
                var->node->next = NULL;
                var->node->prev = NULL;
                
                function->num_locals++;
                function->local_variables = (Variable**) realloc(function->local_variables, sizeof(Variable*)*function->num_locals);
                function->local_variables[function->num_locals-1] = var;
                
                if(stmts_first_not_removed == NULL) {
                    stmts_first_not_removed = n;
                }
            }
            
            if(stmts_first_not_removed == NULL) {
                stmts_first_not_removed = n;
            }
            
            n = n->next;
        }
        function->node->funcdef.stmts = stmts_first_not_removed;
    }
    
    return first_not_removed;
}

Node* FindGlobals(Context *context, Node *nodes) {
    context->global_variables = NULL;
    context->num_globals = 0;
    
    Node *first_not_removed = NULL;
    
    Node *n = nodes;
    while(n != NULL) {
        printf("node type: %d\n", n->type);
        
        if(n->type == NODE_VARDECL) {
            if(n->prev != NULL) n->prev->next = n->next;
            if(n->next != NULL) n->next->prev = n->prev;
                
            Variable *var = new Variable();
            var->node = n;
                
            context->num_globals++;
            context->global_variables = (Variable**) realloc(context->global_variables, sizeof(Variable*)*context->num_globals);
            context->global_variables[context->num_globals-1] = var;
        }else if(n->type == NODE_VARDECLASSIGN) {
            // Replace this with an assign
            // adn remove the decl part
                
            n->type = NODE_ASSIGNMENT;
            char *name = n->vardeclassign.name;
            Node *expr = n->vardeclassign.expr;
            char *type = n->vardeclassign.type;
            n->assignment.name = name;
            n->assignment.expr = expr;
                
            Variable *var = new Variable();
            var->node = new Node();
            var->node->type = NODE_VARDECL;
            var->node->vardecl.name = name;
            var->node->vardecl.type = type;
            var->node->next = NULL;
            var->node->prev = NULL;
            
            context->num_globals++;
            context->global_variables = (Variable**) realloc(context->global_variables, sizeof(Variable*)*context->num_globals);
            context->global_variables[context->num_globals-1] = var;
            
            if(first_not_removed == NULL) {
                first_not_removed = n;
            }
        }
        
        if(first_not_removed == NULL) {
            first_not_removed = n;
        }
        
        n = n->next;
    }
    
    return first_not_removed;
}

void Context::TypeCheck() {
    
}

void Context::Analyse(Node *nodes) {
    
    // Take out struct definitions
    // Do this early so that there is no type resolving
    // do be done at a later point
    // FindStructs();
    
    // Take out function definitions
    nodes = FindFunctions(this, nodes);
    printf("functions: %d\n", num_functions);
    
    // TODO: Semantic analysis inside the function
    // Check things like function call

    // Take out all global declerations
    // but keep assignments
    nodes = FindGlobals(this, nodes);
    printf("globals: %d\n", num_globals);
    
    // TODO: Type-checking
    TypeCheck();
    
    // Nodes might point to something actually removed
    // to fix this, make FindFunctions and FindGlobals
    // return pointer to the first actual node
    // they can do this be checking if they remove the one
    // they get passed
    stmts = nodes;
}