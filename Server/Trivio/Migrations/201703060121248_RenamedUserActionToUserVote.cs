namespace Trivio.Migrations
{
    using System;
    using System.Data.Entity.Migrations;
    
    public partial class RenamedUserActionToUserVote : DbMigration
    {
        public override void Up()
        {
            AddColumn("dbo.UserTriviaHistories", "UserVote", c => c.Int(nullable: false));
            DropColumn("dbo.UserTriviaHistories", "UserAction");
        }
        
        public override void Down()
        {
            AddColumn("dbo.UserTriviaHistories", "UserAction", c => c.Int(nullable: false));
            DropColumn("dbo.UserTriviaHistories", "UserVote");
        }
    }
}
