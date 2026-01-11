#ifndef RIGHTHAND_ASSIGNMENT_H
#define RIGHTHAND_ASSIGNMENT_H

#include "../parser.h"

void check_assignable(Node* node, MessageVector* messages);

Node* parse_postfix_assignment(Node* lefthand, Parser* parser);

#endif