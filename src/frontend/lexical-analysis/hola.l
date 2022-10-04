<MULTILINE_INLINE>{anythingButEndComment}   { ProcessInlineContentPatternAction(); }

<MULTILINE_COMMENT>{anythingButEndComment}   { ProcessCommentContentPatternAction(); }