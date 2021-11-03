/*
 * Copyright (C) 2021 Uniontech Software Technology Co., Ltd.
 *
 * Author:     liuzhangjian<liuzhangjian@uniontech.com>
 *
 * Maintainer: liuzhangjian<liuzhangjian@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef ENABLE_FSEARCH

#include "searcher/file/fssearcher.h"
#include "global/builtinsearch.h"
#include "searcher/file/fsworker.h"
#include "configuration/configer.h"

#include <stubext.h>

#include <gtest/gtest.h>

#include <QTest>

TEST(FsSearcherTest, ut_name)
{
    FsSearcher fs;
    EXPECT_TRUE(fs.name() == GRANDSEARCH_CLASS_FILE_FSEARCH);
}

TEST(FsSearcherTest, ut_isActive)
{
    FsSearcher fs;
    EXPECT_FALSE(fs.isActive());
}

TEST(FsSearcherTest, ut_activate)
{
    FsSearcher fs;
    EXPECT_FALSE(fs.activate());
}

TEST(FsSearcherTest, ut_createWorker)
{
    FsSearcher fs;
    EXPECT_TRUE(fs.createWorker() == nullptr);

    fs.m_isInited = true;
    fs.m_databaseForUpdate = db_new();
    fs.m_app = static_cast<FsearchApplication *>(calloc(1, sizeof(FsearchApplication)));
    fs.m_app->db = db_new();
    fs.m_databaseForUpdate->timestamp = 100;
    fs.m_app->db->timestamp = 0;
    fs.m_isUpdating = false;

    stub_ext::StubExt st;
    //空配置，测试默认值
    UserPreferencePointer upp(new UserPreference(QVariantHash()));
    st.set_lamda(&Configer::group, [&upp](Configer *, const QString &name) {
        if (name == GRANDSEARCH_CLASS_FILE_FSEARCH)
            return upp;

        return UserPreferencePointer();
    });
    st.set_lamda(&QTime::elapsed, [](){ return 11 * 1000; });
    st.set_lamda(&FsSearcher::updateDataBase, [](){ return; });

    auto worker = fs.createWorker();
    EXPECT_TRUE(worker != nullptr);
    EXPECT_NO_FATAL_FAILURE(db_clear(fs.m_app->db));
    EXPECT_NO_FATAL_FAILURE(db_free(fs.m_app->db));
    EXPECT_NO_FATAL_FAILURE(free(fs.m_app));
    EXPECT_NO_FATAL_FAILURE(fs.m_app = nullptr);
    EXPECT_NO_FATAL_FAILURE(delete worker);
}

TEST(FsSearcherTest, ut_action)
{
    FsSearcher fs;
    EXPECT_FALSE(fs.action("", ""));
}

TEST(FsSearcherTest, ut_asyncInitDataBase_0)
{
    FsSearcher fs;
    fs.m_isInited = true;
    EXPECT_NO_FATAL_FAILURE(fs.asyncInitDataBase());
}

TEST(FsSearcherTest, ut_asyncInitDataBase_1)
{
    stub_ext::StubExt st;
    st.set_lamda(&FsSearcher::loadDataBase, [](){ return; });

    FsSearcher fs;
    EXPECT_NO_FATAL_FAILURE(fs.asyncInitDataBase());
}

TEST(FsSearcherTest, ut_loadDataBase)
{
    stub_ext::StubExt st;
    st.set_lamda(load_database, [](Database **db, const char *){
        *db = db_new();
        return nullptr;
    });
    st.set_lamda(db_get_num_entries, [](){ return 0; });
    st.set_lamda(fsearch_thread_pool_init, [](){ return nullptr; });

    FsSearcher fs;
    EXPECT_NO_FATAL_FAILURE(fs.loadDataBase(&fs));
}

TEST(FsSearcherTest, ut_updateDataBase)
{
    stub_ext::StubExt st;
    st.set_lamda(load_database, [](){ return nullptr; });
    st.set_lamda(&FsSearcher::saveDataBase, [](){ return; });

    FsSearcher fs;
    EXPECT_NO_FATAL_FAILURE(fs.updateDataBase(&fs));
}

TEST(FsSearcherTest, ut_saveDataBase)
{
    stub_ext::StubExt st;
    st.set_lamda(&QTime::elapsed, [](){ return 11 * 60 * 1000; });
    st.set_lamda(db_save_locations, [](){ return true; });

    FsSearcher fs;
    EXPECT_NO_FATAL_FAILURE(fs.saveDataBase(nullptr));
}

#endif