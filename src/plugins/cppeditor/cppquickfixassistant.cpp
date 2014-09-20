/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "cppquickfixassistant.h"

#include "cppeditorconstants.h"
#include "cppeditor.h"

#include <cplusplus/ASTPath.h>

#include <extensionsystem/pluginmanager.h>
#include <utils/qtcassert.h>

using namespace TextEditor;
using namespace CppTools;
using namespace CPlusPlus;

namespace CppEditor {
namespace Internal {

// -------------------------
// CppQuickFixAssistProvider
// -------------------------
bool CppQuickFixAssistProvider::isAsynchronous() const
{
    return false;
}

bool CppQuickFixAssistProvider::supportsEditor(Core::Id editorId) const
{
    return editorId == Constants::CPPEDITOR_ID;
}

IAssistProcessor *CppQuickFixAssistProvider::createProcessor() const
{
    return new CppQuickFixAssistProcessor(this);
}

QList<TextEditor::QuickFixFactory *> CppQuickFixAssistProvider::quickFixFactories() const
{
    QList<TextEditor::QuickFixFactory *> results;
    foreach (CppQuickFixFactory *f, ExtensionSystem::PluginManager::getObjects<CppQuickFixFactory>())
        results.append(f);
    return results;
}

// --------------------------
// CppQuickFixAssistProcessor
// --------------------------
CppQuickFixAssistProcessor::CppQuickFixAssistProcessor(const IAssistProvider *provider)
    : m_provider(provider)
{}

const IAssistProvider *CppQuickFixAssistProcessor::provider() const
{
    return m_provider;
}

// --------------------------
// CppQuickFixAssistInterface
// --------------------------
CppQuickFixInterface::CppQuickFixInterface(CppEditorWidget *editor,
                                                       TextEditor::AssistReason reason)
    : AssistInterface(editor->document(), editor->position(),
                      editor->textDocument()->filePath(), reason)
    , m_editor(editor)
    , m_semanticInfo(editor->semanticInfo())
    , m_snapshot(CppTools::CppModelManager::instance()->snapshot())
    , m_currentFile(CppRefactoringChanges::file(editor, m_semanticInfo.doc))
    , m_context(m_semanticInfo.doc, m_snapshot)
{
    QTC_CHECK(m_semanticInfo.doc);
    QTC_CHECK(m_semanticInfo.doc->translationUnit());
    QTC_CHECK(m_semanticInfo.doc->translationUnit()->ast());
    CPlusPlus::ASTPath astPath(m_semanticInfo.doc);
    m_path = astPath(editor->textCursor());
}

const QList<AST *> &CppQuickFixInterface::path() const
{
    return m_path;
}

Snapshot CppQuickFixInterface::snapshot() const
{
    return m_snapshot;
}

SemanticInfo CppQuickFixInterface::semanticInfo() const
{
    return m_semanticInfo;
}

const LookupContext &CppQuickFixInterface::context() const
{
    return m_context;
}

CppEditorWidget *CppQuickFixInterface::editor() const
{
    return m_editor;
}

CppRefactoringFilePtr CppQuickFixInterface::currentFile() const
{
    return m_currentFile;
}

bool CppQuickFixInterface::isCursorOn(unsigned tokenIndex) const
{
    return currentFile()->isCursorOn(tokenIndex);
}

bool CppQuickFixInterface::isCursorOn(const CPlusPlus::AST *ast) const
{
    return currentFile()->isCursorOn(ast);
}

} // namespace Internal
} // namespace CppEditor
