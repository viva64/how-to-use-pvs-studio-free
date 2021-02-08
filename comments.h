//  2006-2008 (c) Viva64.com Team
//  2008-2020 (c) OOO "Program Verification Systems"
//  2020-2021 (c) PVS-Studio LLC

#ifndef PVS_STUDIO_FREE_COMMENTS_COMMENTS_H
#define PVS_STUDIO_FREE_COMMENTS_COMMENTS_H

#include <vector>
#include <string>

namespace PvsStudioFreeComments
{
  struct Comment
  {
    Comment() = default;
    Comment(const char* t);

    bool addLine(const char *begin, const char *end);
    
    std::string m_text;
    std::string m_trimmedText;
    const char *m_begin = nullptr;
    const char *m_end = nullptr;
  };

  typedef std::vector<Comment> FreeComments;

  class CommentsParser
  {

    enum class State : uint8_t
    {
      Unknown,
      Oneline,
      Multiline,
    };

    Comment m_comment;
    static constexpr size_t MaxSkippedLines = 10;
    size_t m_skippedLines                   = 0;
    State m_state                           = State::Unknown;

    void changeState(State state);
    const char* skipNewLine(const char *it);
    const char* readComment(const char *it);
    const char* readOnelineComment(const char *it);
    const char* readMultilineComment(const char *it);
  
  public:
    FreeComments::const_iterator readFreeComment(const char* buf);
    const char* freeCommentBegin() const { return m_comment.m_begin; }
    const char* freeCommentEnd() const { return m_comment.m_end; }
   
  };

  extern const FreeComments Comments;
  bool HasAnyComment(const char* buf);
}

#endif

