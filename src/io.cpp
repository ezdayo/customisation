/**
 *
 * @file      src/io.cpp
 *
 * @brief     This is the implementation of a helper class for Customisation
 *            components serialisation and input-output
 *
 *            This file is part of the Customisation framework (see link).
 *
 * @author    Olivier Stoltz-Douchet <ezdayo@gmail.com>
 *
 * @copyright (c) 2019-2020 Olivier Stoltz-Douchet
 * @license   http://opensource.org/licenses/MIT MIT
 * @link      https://github.com/ezdayo/customisation
 *
 **/

#include "customisation/io.hpp"
#include <iterator>
#include <fstream>

#undef LOGTAG
#define LOGTAG "Customisation::IO"
#include "log.hpp"

namespace Customisation {
namespace IO {

using size_type = std::string::size_type;

std::vector<std::string> loaded(const std::string &name,
                                Error &error) noexcept {
    std::vector<std::string> lines;
    std::ifstream file(name, std::ios_base::in);

    if (!file.is_open()) {
        error = Error::NOT_EXISTING;
        return lines;
    }
    
    bool append = false;
    for (std::string line; std::getline(file, line); /**/) {
        if (line.empty()) {
            append = false;
            continue;
        }
        
        bool append_next = ( (!line.empty()) && (line.back() == '\\') );
        if (append_next) {
            line.pop_back();
        }

        if (append) {
            lines.back().append(line);
        } else {
            lines.emplace_back(line);
        }
        append = append_next;
    }

    error = Error::NONE;
    return lines;
}

Error save(const std::string &name,
           const std::vector<std::string> &lines) noexcept {
    std::ofstream file(name, std::ios_base::out);

    if (!file.is_open()) {
        return Error::INVALID_VALUE;
    }

    std::copy(lines.begin(), lines.end(),
              std::ostream_iterator<std::string>(file, "\n"));
    return Error::NONE;
}

static Error parse(const std::string &line,
                   std::list<std::string> &tokens, int min,
                   int max, size_type pos, size_type end) noexcept {

    std::string token;

    /* When there is only one slot for tokens, put the rest of the line in the
     * last slot ! */
    if (max == 1) {
        while ( (pos < end) && ( (line[pos] == ' ') || (line[pos] == '\t') ) ) {
            ++pos;
        }

        token = std::move(line.substr(pos, end-pos));
        while ( (!token.empty()) &&
                ( (token.back() == ' ') || (token.back() == '\t') ) ) {
            token.pop_back();
        }

        if (!token.empty()) {
            tokens.emplace_back(std::move(token));
        }

        return Error::NONE;
    }

    bool        token_started  = false;
    bool        inside_quotes  = false;
    bool        inside_squotes = false;
    bool        had_backslash  = false;

    for (size_type i = pos; i < end;  ++i) {
        auto c = line[i];
      
        /* If the previous character was a backslash, then copy this character
         * unconditionally */
        if (had_backslash) {
            ASSERT(token_started, "Shall never happen!");
            had_backslash = false;
            token += c;
            continue;
        } 

        /* If we are inside quotes then we are in a token and we solely have to
         * append any new character to the current token and set the 
         * inside_quote status correctly */ 
        if ( (inside_quotes) || (inside_squotes) ) {
            ASSERT(token_started, "Shall never happen!");
            if ( (c == '"') && (inside_quotes) ) {
                inside_quotes = false;
            }
            
            if ( (c == '\'') && (inside_squotes) ) {
                inside_squotes = false;
            }

            had_backslash = (c == '\\');
            if (!had_backslash) { 
                token += c;
            }
            continue;
        }

        switch(c) {
            /* Managing token separators */
            case ',':
            case '(':
            case '=':
            case ')':
                if (token_started) {
                    tokens.emplace_back(std::move(token));
                    return parse(line, tokens, min-1, max-1, i, end);
                }
                token += c;
                tokens.emplace_back(std::move(token));
                return parse(line, tokens, min-1, max-1, i+1, end);
                break;

            /* Managing spacing separators */
            case ' ':
            case '\t':
                if (token_started) {
                    tokens.emplace_back(std::move(token));
                    return parse(line, tokens, min -1, max-1, i+1, end);
                }
                break;

            /* This is the beginning of a valid comment */
            case '#':
                i = end;
                break;

            /* Detect backslash or quote to enter their shortcut */
            case '\\':
                had_backslash  = (c == '\\');
                token_started = true;
                break;

            case '\'':
            case '"':
                inside_squotes = (c == '\'');
                inside_quotes  = (c == '"');
                /* This is an explicit fallthrough! */
                goto addtoken;

            /* Otherwise, we either start a new token or continue an existing 
             * one */
            default:
            addtoken:
                token_started = true;
                token += c;
                break;
        }
    }
     
    /* If still inside quotes or if we did not manage to extract the requested
     * amount of tokens for a non pure-comment line, then this is an issue ! */
    if ( (inside_quotes) || (inside_squotes) || 
         ( (min > 1) && (! tokens.empty()) ) ) {
        return Error::INVALID_VALUE;
    }

    if (token_started) {
        while ( (!token.empty()) &&
                ( (token.back() == ' ') || (token.back() == '\t') ) ) {
            token.pop_back();
        }

        if (!token.empty()) {
            tokens.emplace_back(std::move(token));
        }
    }

    return Error::NONE;
}

Error parse(const std::string &line,
               std::list<std::string> &tokens, int min, int max) noexcept {
    return parse(line, tokens, min, max, 0, line.size());
}

Error parse(const std::string &line,
               std::list<std::string> &tokens, int max) noexcept {
    return parse(line, tokens, -1, max);
}

Error parse(const std::string &line,
            std::list<std::string> &tokens) noexcept {
    return parse(line, tokens, -1);
}

void compose(std::vector<std::string> &output, std::string path,
             const std::string &value, const std::string &comment, 
             const std::string &description) noexcept {

    int psize = path.length() + 3;
    int vsize = value.length();
    int csize = (comment.empty()) ? 0 : comment.length() + 3;

    /* Put the description (if any) in a dedicated line above */
    if (!description.empty()) {
        output.emplace_back("# ");
        output.back().append(description);
    }

    /* Start a new line with the path and append a ' = ' */
    output.emplace_back(std::move(path));
    output.back().append(" = ");

    /* Put everything on a single 80-character line if possible */
    if ((psize + vsize + csize) <= 80) {
        output.back().append(value);
        if (csize > 0) {
            output.back().append(" # ");
            output.back().append(comment);
        }

        return;
    }

    /* If only the comment cannot make it then simply put the comment on a
     * following line starting with a tabulation */
    if ((psize + vsize) <= 80) {
        output.back().append(value);
        output.emplace_back("\t# ");
        output.back().append(comment);

        return;
    }

    /* The value does not fit in the 80-character line so lets put it on a new
     * line */
    output.back().append(" \\");
    output.emplace_back("\t");
    output.back().append(value);

    /* If there is no comment, we are done */
    if (csize == 0) {
        return;
    }

    /* Put the comment on the same line, only if it fits ! */
    if ((7 + vsize + csize) <= 80) {
        output.back().append(" # ");
    } else {
        output.emplace_back("\t# ");
    }   
    output.back().append(comment);
}

}  // namespace IO
}  // namespace Customisation
